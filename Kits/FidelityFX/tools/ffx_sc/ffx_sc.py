#!/usr/bin/env python3
"""
FidelityFX Shader Compiler - Linux Native Port
================================================
Drop-in replacement for FidelityFX_SC.exe that runs natively on Linux.
Compiles HLSL shaders to SPIR-V using DXC and generates permutation headers
with reflection data, matching the exact output format of the Windows tool.

Copyright (C) 2025 Advanced Micro Devices, Inc.  (original)
Linux port by Kelsi Davis, 2026.

Usage:
    ffx_sc.py [options] <InputFile.hlsl>

Options mirror the original FidelityFX_SC.exe. See --help for details.
"""

import argparse
import hashlib
import itertools
import os
import struct
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor, as_completed
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Tuple

# ---------------------------------------------------------------------------
# SPIR-V constants (from spirv.h)
# ---------------------------------------------------------------------------
SPIRV_MAGIC = 0x07230203

# Opcodes we care about for reflection
OP_NAME = 5
OP_DECORATE = 71
OP_TYPE_VOID = 19
OP_TYPE_BOOL = 20
OP_TYPE_INT = 21
OP_TYPE_FLOAT = 22
OP_TYPE_VECTOR = 23
OP_TYPE_MATRIX = 24
OP_TYPE_IMAGE = 25
OP_TYPE_SAMPLER = 26
OP_TYPE_SAMPLED_IMAGE = 27
OP_TYPE_ARRAY = 28
OP_TYPE_RUNTIME_ARRAY = 29
OP_TYPE_STRUCT = 30
OP_TYPE_POINTER = 32
OP_TYPE_ACCELERATION_STRUCTURE_KHR = 5341
OP_VARIABLE = 59

# Decoration IDs
DECORATION_BINDING = 33
DECORATION_DESCRIPTOR_SET = 34

# Storage classes
STORAGE_CLASS_UNIFORM_CONSTANT = 0
STORAGE_CLASS_INPUT = 1
STORAGE_CLASS_UNIFORM = 2
STORAGE_CLASS_PUSH_CONSTANT = 9
STORAGE_CLASS_STORAGE_BUFFER = 12

# Image Dim
DIM_1D = 0
DIM_2D = 1
DIM_3D = 2
DIM_CUBE = 3
DIM_RECT = 4
DIM_BUFFER = 5
DIM_SUBPASS_DATA = 6


@dataclass
class ShaderResourceInfo:
    name: str
    binding: int
    count: int
    space: int


@dataclass
class ReflectionData:
    constant_buffers: List[ShaderResourceInfo] = field(default_factory=list)
    srv_textures: List[ShaderResourceInfo] = field(default_factory=list)
    uav_textures: List[ShaderResourceInfo] = field(default_factory=list)
    srv_buffers: List[ShaderResourceInfo] = field(default_factory=list)
    uav_buffers: List[ShaderResourceInfo] = field(default_factory=list)
    samplers: List[ShaderResourceInfo] = field(default_factory=list)
    rt_accel_structs: List[ShaderResourceInfo] = field(default_factory=list)


@dataclass
class PermutationOption:
    definition: str
    values: List[str]
    num_bits: int
    is_numeric: bool


@dataclass
class Permutation:
    key: int = 0
    hash_digest: str = ""
    name: str = ""
    header_filename: str = ""
    defines: List[Tuple[str, str]] = field(default_factory=list)
    shader_binary: Optional[bytes] = None
    reflection: Optional[ReflectionData] = None


# ---------------------------------------------------------------------------
# SPIR-V reflection parser
# ---------------------------------------------------------------------------
def parse_spirv_reflection(spirv_data: bytes) -> ReflectionData:
    """Parse SPIR-V binary to extract resource binding reflection data."""
    if len(spirv_data) < 20:
        return ReflectionData()

    words = struct.unpack(f"<{len(spirv_data) // 4}I", spirv_data)

    if words[0] != SPIRV_MAGIC:
        raise ValueError(f"Not a valid SPIR-V binary (magic: 0x{words[0]:08x})")

    # Collect metadata in passes
    names: Dict[int, str] = {}
    bindings: Dict[int, int] = {}
    descriptor_sets: Dict[int, int] = {}
    types: Dict[int, tuple] = {}  # id -> (opcode, operands...)
    variables: Dict[int, tuple] = {}  # id -> (type_id, storage_class)

    i = 5  # Skip header
    while i < len(words):
        word_count = words[i] >> 16
        opcode = words[i] & 0xFFFF

        if word_count == 0:
            break

        operands = words[i + 1 : i + word_count] if word_count > 1 else []

        if opcode == OP_NAME and len(operands) >= 2:
            target_id = operands[0]
            # Decode name string from remaining words
            name_bytes = struct.pack(f"<{len(operands) - 1}I", *operands[1:])
            null_pos = name_bytes.find(b"\x00")
            if null_pos >= 0:
                name_bytes = name_bytes[:null_pos]
            names[target_id] = name_bytes.decode("utf-8", errors="replace")

        elif opcode == OP_DECORATE and len(operands) >= 3:
            target_id = operands[0]
            decoration = operands[1]
            if decoration == DECORATION_BINDING:
                bindings[target_id] = operands[2]
            elif decoration == DECORATION_DESCRIPTOR_SET:
                descriptor_sets[target_id] = operands[2]

        elif opcode in (
            OP_TYPE_VOID, OP_TYPE_BOOL, OP_TYPE_INT, OP_TYPE_FLOAT,
            OP_TYPE_VECTOR, OP_TYPE_MATRIX, OP_TYPE_IMAGE, OP_TYPE_SAMPLER,
            OP_TYPE_SAMPLED_IMAGE, OP_TYPE_ARRAY, OP_TYPE_RUNTIME_ARRAY,
            OP_TYPE_STRUCT, OP_TYPE_POINTER, OP_TYPE_ACCELERATION_STRUCTURE_KHR,
        ):
            if len(operands) >= 1:
                types[operands[0]] = (opcode, *operands[1:])

        elif opcode == OP_VARIABLE and len(operands) >= 3:
            result_type = operands[0]
            result_id = operands[1]
            storage_class = operands[2]
            variables[result_id] = (result_type, storage_class)

        i += word_count

    # Now classify each variable with a binding
    reflection = ReflectionData()

    for var_id, (ptr_type_id, storage_class) in variables.items():
        if var_id not in bindings:
            continue
        if storage_class == STORAGE_CLASS_PUSH_CONSTANT:
            continue
        if storage_class == STORAGE_CLASS_INPUT:
            continue

        binding = bindings.get(var_id, 0)
        desc_set = descriptor_sets.get(var_id, 0)
        name = names.get(var_id, f"unnamed_{var_id}")

        # Resolve pointer type to get the actual type
        actual_type = _resolve_type(ptr_type_id, types)

        info = ShaderResourceInfo(name=name, binding=binding, count=1, space=desc_set)

        if actual_type is None:
            continue

        opcode = actual_type[0]

        if opcode == OP_TYPE_ACCELERATION_STRUCTURE_KHR:
            reflection.rt_accel_structs.append(info)
        elif opcode == OP_TYPE_SAMPLER:
            reflection.samplers.append(info)
        elif opcode == OP_TYPE_SAMPLED_IMAGE:
            reflection.srv_textures.append(info)
        elif opcode == OP_TYPE_IMAGE:
            # Image: operands are (sampled_type, dim, depth, arrayed, ms, sampled, format, ...)
            if len(actual_type) >= 7:
                dim = actual_type[2]
                sampled = actual_type[6]
                if dim == DIM_BUFFER:
                    # Buffer image
                    if sampled == 2:
                        reflection.uav_buffers.append(info)
                    else:
                        reflection.srv_buffers.append(info)
                else:
                    # Texture image
                    if sampled == 2:
                        reflection.uav_textures.append(info)
                    else:
                        reflection.srv_textures.append(info)
            else:
                reflection.srv_textures.append(info)
        elif opcode == OP_TYPE_STRUCT:
            # Struct in Uniform storage class = CBV (uniform buffer)
            # Struct in StorageBuffer = SRV/UAV buffer
            if storage_class == STORAGE_CLASS_UNIFORM:
                reflection.constant_buffers.append(info)
            elif storage_class == STORAGE_CLASS_STORAGE_BUFFER:
                # Heuristic: if name contains "rw" or "RW", it's UAV
                if "rw_" in name.lower() or name.startswith("RW"):
                    reflection.uav_buffers.append(info)
                else:
                    reflection.srv_buffers.append(info)
        # Arrays of images/samplers handled by the count field (TODO if needed)

    return reflection


def _resolve_type(type_id: int, types: Dict[int, tuple]) -> Optional[tuple]:
    """Follow pointer/array type chains to find the underlying type."""
    seen = set()
    current = type_id
    while current in types and current not in seen:
        seen.add(current)
        info = types[current]
        opcode = info[0]
        if opcode == OP_TYPE_POINTER and len(info) >= 3:
            current = info[2]  # pointee type
        elif opcode == OP_TYPE_ARRAY and len(info) >= 2:
            current = info[1]  # element type
        elif opcode == OP_TYPE_RUNTIME_ARRAY and len(info) >= 2:
            current = info[1]  # element type
        else:
            return info
    if current in types:
        return types[current]
    return None


# ---------------------------------------------------------------------------
# Compilation
# ---------------------------------------------------------------------------
def find_dxc() -> str:
    """Find the DXC binary."""
    # Check environment
    dxc_env = os.environ.get("DXC")
    if dxc_env and os.path.isfile(dxc_env):
        return dxc_env

    # Check common locations
    candidates = [
        "/tmp/dxc/bin/dxc",
        os.path.expanduser("~/.local/bin/dxc"),
        "/usr/bin/dxc",
        "/usr/local/bin/dxc",
    ]
    for c in candidates:
        if os.path.isfile(c):
            return c

    # Fall back to PATH
    import shutil
    found = shutil.which("dxc")
    if found:
        return found

    raise FileNotFoundError(
        "DXC not found. Install it or set DXC=/path/to/dxc. "
        "Download from: https://github.com/microsoft/DirectXShaderCompiler/releases"
    )


def compile_hlsl_to_spirv(
    dxc_path: str,
    input_file: str,
    entry_point: str,
    target_profile: str,
    defines: List[Tuple[str, str]],
    include_dirs: List[str],
    extra_args: List[str],
) -> bytes:
    """Compile an HLSL file to SPIR-V using DXC and return the binary data."""
    with tempfile.NamedTemporaryFile(suffix=".spv", delete=False) as tmp:
        output_path = tmp.name

    try:
        cmd = [dxc_path]
        cmd += ["-E", entry_point]
        cmd += ["-T", target_profile]
        cmd += ["-spirv", "-fvk-use-dx-layout", "-fspv-target-env=vulkan1.1"]
        cmd += ["-Fo", output_path]

        for name, value in defines:
            if value is not None:
                cmd += [f"-D{name}={value}"]
            else:
                cmd += [f"-D{name}"]

        for inc in include_dirs:
            cmd += ["-I", inc]

        # Filter out SPIR-V and target args from extra_args (we set them ourselves)
        skip_next = False
        for arg in extra_args:
            if skip_next:
                skip_next = False
                continue
            if arg in ("-spirv", "-fvk-use-dx-layout") or arg.startswith("-fspv-target-env"):
                continue
            if arg in ("-E", "-T", "-Fo"):
                skip_next = True
                continue
            if arg == "-Zs":
                continue  # Skip validation-only flag
            cmd.append(arg)

        cmd.append(input_file)

        env = os.environ.copy()
        # DXC needs to find its shared library (libdxcompiler.so/.dylib)
        dxc_dir = os.path.dirname(dxc_path)
        lib_dir = os.path.join(dxc_dir, "..", "lib")
        if os.path.isdir(lib_dir):
            if sys.platform == "darwin":
                env["DYLD_LIBRARY_PATH"] = lib_dir + ":" + env.get("DYLD_LIBRARY_PATH", "")
            else:
                env["LD_LIBRARY_PATH"] = lib_dir + ":" + env.get("LD_LIBRARY_PATH", "")

        result = subprocess.run(
            cmd, capture_output=True, text=True, timeout=120, env=env
        )

        if result.returncode != 0:
            # Print warnings but not full error for redefined macros
            stderr = result.stderr
            if stderr:
                for line in stderr.split("\n"):
                    if "error" in line.lower():
                        print(f"    ERROR: {line}", file=sys.stderr)
            raise RuntimeError(
                f"DXC compilation failed (exit {result.returncode})\n{result.stderr}"
            )

        with open(output_path, "rb") as f:
            return f.read()
    finally:
        if os.path.exists(output_path):
            os.unlink(output_path)


# ---------------------------------------------------------------------------
# Permutation generation
# ---------------------------------------------------------------------------
def parse_permutation_options(raw_defines: List[str]) -> Tuple[List[PermutationOption], List[Tuple[str, str]]]:
    """Parse -D arguments, separating permutation options from fixed defines."""
    perm_options = []
    fixed_defines = []

    for d in raw_defines:
        if "={" in d and d.endswith("}"):
            # Permutation option: NAME={val1,val2,...}
            eq_pos = d.index("=")
            name = d[:eq_pos]
            values_str = d[eq_pos + 2 : -1]  # strip ={ and }
            values = [v.strip() for v in values_str.split(",")]

            is_numeric = all(v.isdigit() for v in values)
            num_bits = max(1, (len(values) - 1).bit_length())

            perm_options.append(
                PermutationOption(
                    definition=name, values=values, num_bits=num_bits, is_numeric=is_numeric
                )
            )
        elif "=" in d:
            name, value = d.split("=", 1)
            fixed_defines.append((name, value))
        else:
            fixed_defines.append((d, None))

    return perm_options, fixed_defines


def generate_permutations(
    perm_options: List[PermutationOption],
) -> List[Tuple[int, List[Tuple[str, str]]]]:
    """Generate all permutation combinations with their keys."""
    if not perm_options:
        return [(0, [])]

    all_value_lists = [opt.values for opt in perm_options]
    results = []

    for combo in itertools.product(*all_value_lists):
        key = 0
        bit_offset = 0
        defines = []
        for opt, val in zip(perm_options, combo):
            val_index = opt.values.index(val)
            key |= val_index << bit_offset
            bit_offset += opt.num_bits
            defines.append((opt.definition, val))
        results.append((key, defines))

    return results


# ---------------------------------------------------------------------------
# Header generation
# ---------------------------------------------------------------------------
def write_binary_header(
    output_dir: str,
    shader_name: str,
    permutation: Permutation,
    embed_arguments: bool,
    compiler_args: List[str],
) -> None:
    """Write a per-permutation binary header file."""
    perm_name = f"{shader_name}_{permutation.hash_digest}"
    filename = f"{perm_name}.h"
    filepath = os.path.join(output_dir, filename)
    permutation.header_filename = filename

    with open(filepath, "w") as fp:
        fp.write(f"// {perm_name}.h.\n")
        fp.write("// Auto generated by FidelityFX-SC (Linux).\n\n")

        if embed_arguments:
            for arg in compiler_args:
                if arg.startswith("-"):
                    fp.write(f"\n// {arg}")
                    if not arg.startswith("-D"):
                        fp.write(" ")
                else:
                    fp.write(arg)
            for name, val in permutation.defines:
                if val is not None:
                    fp.write(f"\n// -D{name}={val}")
                else:
                    fp.write(f"\n// -D{name}")
            fp.write("\n\n")

        # Write reflection data
        if permutation.reflection:
            _write_reflection_arrays(fp, perm_name, permutation.reflection)

        # Write shader binary
        data = permutation.shader_binary
        fp.write(f"static const uint32_t g_{perm_name}_size = {len(data)};\n\n")
        fp.write(f"static const unsigned char g_{perm_name}_data[] = {{\n")

        for idx, byte in enumerate(data):
            is_last = idx == len(data) - 1
            sep = "" if is_last else (",\n" if (idx + 1) % 16 == 0 else ",")
            fp.write(f"0x{byte:02x}{sep}")

        fp.write("\n};\n\n")


def _write_reflection_arrays(fp, perm_name: str, refl: ReflectionData) -> None:
    """Write reflection data arrays for a single permutation."""
    resource_types = [
        ("CBV", refl.constant_buffers),
        ("TextureSRV", refl.srv_textures),
        ("TextureUAV", refl.uav_textures),
        ("BufferSRV", refl.srv_buffers),
        ("BufferUAV", refl.uav_buffers),
        ("Sampler", refl.samplers),
        ("RTAccelerationStructure", refl.rt_accel_structs),
    ]

    for type_name, resources in resource_types:
        if not resources:
            continue

        fp.write(f'static const char* g_{perm_name}_{type_name}ResourceNames[] = {{ ')
        fp.write(", ".join(f'"{r.name}"' for r in resources))
        fp.write(" };\n")

        fp.write(f"static const uint32_t g_{perm_name}_{type_name}ResourceBindings[] = {{ ")
        fp.write(", ".join(f" {r.binding}" for r in resources))
        fp.write(" };\n")

        fp.write(f"static const uint32_t g_{perm_name}_{type_name}ResourceCounts[] = {{ ")
        fp.write(", ".join(f" {r.count}" for r in resources))
        fp.write(" };\n")

        fp.write(f"static const uint32_t g_{perm_name}_{type_name}ResourceSpaces[] = {{ ")
        fp.write(", ".join(f" {r.space}" for r in resources))
        fp.write(" };\n\n")


def write_permutations_header(
    output_dir: str,
    shader_name: str,
    perm_options: List[PermutationOption],
    unique_perms: List[Permutation],
    key_to_index: Dict[int, int],
    generate_reflection: bool,
) -> None:
    """Write the master permutations header file."""
    filepath = os.path.join(output_dir, f"{shader_name}_permutations.h")

    with open(filepath, "w") as fp:
        # Include per-permutation headers
        for perm in unique_perms:
            fp.write(f'#include "{perm.header_filename}"\n')
        fp.write("\n")

        # Write enum types for non-numeric options
        for opt in perm_options:
            if not opt.is_numeric:
                enum_name = opt.definition
                fp.write(f"typedef enum {enum_name} {{\n")
                for j, val in enumerate(opt.values):
                    upper_name = enum_name.upper()
                    upper_val = val.upper()
                    suffix = "" if j == len(opt.values) - 1 else ","
                    fp.write(f"    OPT_{upper_name}_{upper_val} = {j}{suffix}\n")
                fp.write(f"}} {enum_name};\n\n")

        # Write permutation key union
        union_name = f"{shader_name}_PermutationKey"
        fp.write(f"typedef union {union_name} {{\n")
        fp.write("    struct {\n")
        for opt in perm_options:
            fp.write(f"        uint32_t {opt.definition} : {opt.num_bits};\n")
        fp.write("    };\n")
        fp.write("    uint32_t index;\n")
        fp.write(f"}} {union_name};\n\n")

        # Write permutation info struct
        fp.write(f"typedef struct {shader_name}_PermutationInfo {{\n")
        fp.write("    const uint32_t       blobSize;\n")
        fp.write("    const unsigned char* blobData;\n\n")

        if generate_reflection:
            fp.write("\n")
            fp.write("    const uint32_t  numConstantBuffers;\n")
            fp.write("    const char**    constantBufferNames;\n")
            fp.write("    const uint32_t* constantBufferBindings;\n")
            fp.write("    const uint32_t* constantBufferCounts;\n")
            fp.write("    const uint32_t* constantBufferSpaces;\n\n")
            fp.write("    const uint32_t  numSRVTextures;\n")
            fp.write("    const char**    srvTextureNames;\n")
            fp.write("    const uint32_t* srvTextureBindings;\n")
            fp.write("    const uint32_t* srvTextureCounts;\n")
            fp.write("    const uint32_t* srvTextureSpaces;\n\n")
            fp.write("    const uint32_t  numUAVTextures;\n")
            fp.write("    const char**    uavTextureNames;\n")
            fp.write("    const uint32_t* uavTextureBindings;\n")
            fp.write("    const uint32_t* uavTextureCounts;\n")
            fp.write("    const uint32_t* uavTextureSpaces;\n\n")
            fp.write("    const uint32_t  numSRVBuffers;\n")
            fp.write("    const char**    srvBufferNames;\n")
            fp.write("    const uint32_t* srvBufferBindings;\n")
            fp.write("    const uint32_t* srvBufferCounts;\n")
            fp.write("    const uint32_t* srvBufferSpaces;\n\n")
            fp.write("    const uint32_t  numUAVBuffers;\n")
            fp.write("    const char**    uavBufferNames;\n")
            fp.write("    const uint32_t* uavBufferBindings;\n")
            fp.write("    const uint32_t* uavBufferCounts;\n")
            fp.write("    const uint32_t* uavBufferSpaces;\n\n")
            fp.write("    const uint32_t  numSamplers;\n")
            fp.write("    const char**    samplerNames;\n")
            fp.write("    const uint32_t* samplerBindings;\n")
            fp.write("    const uint32_t* samplerCounts;\n")
            fp.write("    const uint32_t* samplerSpaces;\n\n")
            fp.write("    const uint32_t  numRTAccelerationStructures;\n")
            fp.write("    const char**    rtAccelerationStructureNames;\n")
            fp.write("    const uint32_t* rtAccelerationStructureBindings;\n")
            fp.write("    const uint32_t* rtAccelerationStructureCounts;\n")
            fp.write("    const uint32_t* rtAccelerationStructureSpaces;\n")

        fp.write(f"}} {shader_name}_PermutationInfo;\n\n")

        # Write indirection table
        total_bits = sum(opt.num_bits for opt in perm_options)
        total_entries = 1 << total_bits if perm_options else 1

        fp.write(f"static const uint32_t g_{shader_name}_IndirectionTable[] = {{\n")
        for i in range(total_entries):
            fp.write(f"    {key_to_index.get(i, 0)},\n")
        fp.write("};\n\n")

        # Write permutation info table
        fp.write(f"static const {shader_name}_PermutationInfo g_{shader_name}_PermutationInfo[] = {{\n")
        for perm in unique_perms:
            perm_name = f"{shader_name}_{perm.hash_digest}"
            fp.write(f"    {{ g_{perm_name}_size, g_{perm_name}_data, ")

            if generate_reflection and perm.reflection:
                _write_permutation_info_reflection(fp, perm_name, perm.reflection)
            elif generate_reflection:
                fp.write("0, 0, 0, 0, 0, " * 7)

            fp.write("},\n")
        fp.write("};\n\n")


def _write_permutation_info_reflection(fp, perm_name: str, refl: ReflectionData) -> None:
    """Write reflection fields for a single entry in the PermutationInfo table."""
    resource_types = [
        ("CBV", refl.constant_buffers),
        ("TextureSRV", refl.srv_textures),
        ("TextureUAV", refl.uav_textures),
        ("BufferSRV", refl.srv_buffers),
        ("BufferUAV", refl.uav_buffers),
        ("Sampler", refl.samplers),
        ("RTAccelerationStructure", refl.rt_accel_structs),
    ]

    for type_name, resources in resource_types:
        if not resources:
            fp.write("0, 0, 0, 0, 0, ")
        else:
            count = len(resources)
            fp.write(
                f"{count}, "
                f"g_{perm_name}_{type_name}ResourceNames, "
                f"g_{perm_name}_{type_name}ResourceBindings, "
                f"g_{perm_name}_{type_name}ResourceCounts, "
                f"g_{perm_name}_{type_name}ResourceSpaces, "
            )


# ---------------------------------------------------------------------------
# Main application
# ---------------------------------------------------------------------------
def main():
    parser = argparse.ArgumentParser(
        description="FidelityFX Shader Compiler - Linux Native Port",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("input_file", help="HLSL shader source file")
    parser.add_argument("-output", "--output", required=True, help="Output directory")
    parser.add_argument("-name", "--name", default=None, help="Shader name prefix")
    parser.add_argument("-E", dest="entry_point", default="CS", help="Entry point (default: CS)")
    parser.add_argument("-T", dest="target_profile", default="cs_6_2", help="Target profile")
    parser.add_argument("-reflection", action="store_true", help="Generate reflection data")
    parser.add_argument("-embed-arguments", action="store_true", help="Embed compile arguments")
    parser.add_argument("-deps", choices=["gcc", "msvc"], default=None, help="Depfile format")
    parser.add_argument("-num-threads", type=int, default=0, help="Thread count (0=auto)")
    parser.add_argument("-disable-logs", action="store_true", help="Suppress logs")
    parser.add_argument("-Zs", action="store_true", help="Ignored (compat)")
    parser.add_argument("-dxc", default=None, help="Path to DXC binary")
    parser.add_argument(
        "extra_args",
        nargs=argparse.REMAINDER,
        help="Additional compiler arguments (after --)",
    )

    # Pre-process sys.argv to handle FidelityFX_SC.exe-style arguments
    args_list = sys.argv[1:]

    # Separate our arguments from pass-through compiler arguments
    our_args = []
    compiler_args = []
    define_args = []
    include_dirs = []
    input_file = None
    entry_point = "CS"
    target_profile = "cs_6_2"
    output_dir = None
    shader_name = None
    generate_reflection = False
    embed_arguments = False
    deps_format = None
    num_threads = 0
    disable_logs = False
    dxc_path_arg = None

    i = 0
    while i < len(args_list):
        arg = args_list[i]

        if arg.startswith("-D"):
            define_str = arg[2:]
            # Handle -D NAME=VALUE (with space)
            if not define_str and i + 1 < len(args_list):
                i += 1
                define_str = args_list[i]
            define_args.append(define_str)
        elif arg.startswith("-I"):
            inc = arg[2:]
            if not inc and i + 1 < len(args_list):
                i += 1
                inc = args_list[i]
            include_dirs.append(inc)
        elif arg.startswith("-output="):
            output_dir = arg.split("=", 1)[1].strip('"')
        elif arg.startswith("-name="):
            shader_name = arg.split("=", 1)[1].strip('"')
        elif arg == "-reflection":
            generate_reflection = True
        elif arg == "-embed-arguments":
            embed_arguments = True
        elif arg == "-disable-logs":
            disable_logs = True
        elif arg == "-Zs":
            pass  # Ignored
        elif arg.startswith("-deps="):
            deps_format = arg.split("=", 1)[1]
        elif arg.startswith("-num-threads="):
            num_threads = int(arg.split("=", 1)[1])
        elif arg.startswith("-dxc="):
            dxc_path_arg = arg.split("=", 1)[1]
        elif arg == "-E":
            i += 1
            entry_point = args_list[i]
        elif arg == "-T":
            i += 1
            target_profile = args_list[i]
        elif arg.startswith("-"):
            # Pass-through to DXC
            compiler_args.append(arg)
            # Check if next arg is a value for this flag
            if i + 1 < len(args_list) and not args_list[i + 1].startswith("-"):
                # Only consume if it looks like a flag value
                if arg in ("-Fo", "-Fe", "-Fh", "-Fc", "-HV"):
                    i += 1
                    compiler_args.append(args_list[i])
        else:
            input_file = arg

        i += 1

    if not input_file:
        print("ERROR: No input file specified.", file=sys.stderr)
        sys.exit(1)

    if not output_dir:
        print("ERROR: No output directory specified. Use -output=<path>", file=sys.stderr)
        sys.exit(1)

    if not shader_name:
        shader_name = Path(input_file).stem

    # Find DXC
    dxc_path = dxc_path_arg or find_dxc()
    if not disable_logs:
        print(f"{Path(input_file).name}")

    # Parse permutation options
    perm_options, fixed_defines = parse_permutation_options(define_args)

    # Generate all permutation combinations
    perm_combos = generate_permutations(perm_options)

    # Create output directory
    os.makedirs(output_dir, exist_ok=True)

    # Compile all permutations
    unique_perms: List[Permutation] = []
    key_to_index: Dict[int, int] = {}
    hash_to_index: Dict[str, int] = {}

    thread_count = num_threads if num_threads > 0 else min(os.cpu_count() or 4, len(perm_combos))

    def compile_one(key: int, perm_defines: List[Tuple[str, str]]) -> Tuple[int, Optional[Permutation]]:
        all_defines = list(fixed_defines) + perm_defines

        try:
            spirv_data = compile_hlsl_to_spirv(
                dxc_path=dxc_path,
                input_file=input_file,
                entry_point=entry_point,
                target_profile=target_profile,
                defines=all_defines,
                include_dirs=include_dirs,
                extra_args=compiler_args,
            )
        except RuntimeError as e:
            if not disable_logs:
                print(f"  WARNING: Permutation {key} failed: {e}", file=sys.stderr)
            return key, None

        # Hash the binary
        md5 = hashlib.md5(spirv_data).hexdigest()[:16]

        perm = Permutation(
            key=key,
            hash_digest=md5,
            name=f"{shader_name}_{md5}",
            defines=perm_defines,
            shader_binary=spirv_data,
        )

        if generate_reflection:
            try:
                perm.reflection = parse_spirv_reflection(spirv_data)
            except Exception as e:
                if not disable_logs:
                    print(f"  WARNING: Reflection failed for permutation {key}: {e}", file=sys.stderr)
                perm.reflection = ReflectionData()

        return key, perm

    if thread_count > 1 and len(perm_combos) > 1:
        with ThreadPoolExecutor(max_workers=thread_count) as executor:
            futures = {
                executor.submit(compile_one, key, defines): key
                for key, defines in perm_combos
            }
            for future in as_completed(futures):
                key, perm = future.result()
                if perm is None:
                    continue
                if perm.hash_digest in hash_to_index:
                    key_to_index[key] = hash_to_index[perm.hash_digest]
                else:
                    idx = len(unique_perms)
                    unique_perms.append(perm)
                    hash_to_index[perm.hash_digest] = idx
                    key_to_index[key] = idx
    else:
        for key, defines in perm_combos:
            key, perm = compile_one(key, defines)
            if perm is None:
                continue
            if perm.hash_digest in hash_to_index:
                key_to_index[key] = hash_to_index[perm.hash_digest]
            else:
                idx = len(unique_perms)
                unique_perms.append(perm)
                hash_to_index[perm.hash_digest] = idx
                key_to_index[key] = idx

    if not unique_perms:
        print("ERROR: No shader permutations generated!", file=sys.stderr)
        sys.exit(1)

    if not disable_logs:
        print(f"  {len(unique_perms)} unique permutations from {len(perm_combos)} total")

    # Write per-permutation binary headers
    for perm in unique_perms:
        write_binary_header(output_dir, shader_name, perm, embed_arguments, compiler_args)

    # Write master permutations header
    write_permutations_header(
        output_dir, shader_name, perm_options, unique_perms, key_to_index, generate_reflection
    )

    # Write depfile if requested
    if deps_format == "gcc":
        _write_depfile_gcc(output_dir, shader_name)


def _write_depfile_gcc(output_dir: str, shader_name: str) -> None:
    """Write a GCC-format dependency file."""
    output_path = os.path.join(output_dir, f"{shader_name}_permutations.h")
    dep_path = output_path + ".d"
    # Minimal depfile — just the output depending on the permutations header
    with open(dep_path, "w") as fp:
        fp.write(f"{output_path}:\n")


if __name__ == "__main__":
    main()
