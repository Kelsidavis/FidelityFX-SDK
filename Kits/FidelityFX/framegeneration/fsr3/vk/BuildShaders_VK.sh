#!/bin/bash
# This file is part of the FidelityFX SDK.
#
# Copyright (C) 2025 Advanced Micro Devices, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# Build FidelityFX frame generation shaders for Vulkan (HLSL -> SPIR-V)
#
# Usage: ./BuildShaders_VK.sh [output_dir]
#
# Requires: DXC (DirectX Shader Compiler) with SPIR-V support
#   Install via: apt install dxc  -or-  from https://github.com/microsoft/DirectXShaderCompiler
#
# This script compiles the frame interpolation and optical flow HLSL compute
# shaders into SPIR-V permutation headers that the VK backend links against.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SDK_DIR="$(cd "$SCRIPT_DIR/../../.." && pwd)"
SHADER_DIR="$SDK_DIR/framegeneration/fsr3/internal/shaders"
OUTPUT_DIR="${1:-$SCRIPT_DIR/spirv_output}"

DXC="${DXC:-dxc}"

# Verify DXC is available
if ! command -v "$DXC" &> /dev/null; then
    echo "ERROR: DXC (DirectX Shader Compiler) not found."
    echo "Install via: apt install dxc"
    echo "Or set DXC=/path/to/dxc"
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

INCLUDE_ARGS="-I $SDK_DIR/api/internal/include/gpu -I $SDK_DIR/framegeneration/fsr3/include/gpu"
BASE_ARGS="-DFFX_GPU=1 -DFFX_HLSL=1 -DFFX_GLSL=0 -DFFX_IMPLICIT_SHADER_REGISTER_BINDING_HLSL=0"
SPIRV_ARGS="-spirv -fvk-use-dx-layout -fspv-target-env=vulkan1.1"

# Frame interpolation shader list
FI_SHADERS=(
    "ffx_frameinterpolation_reconstruct_and_dilate_pass"
    "ffx_frameinterpolation_disocclusion_mask_pass"
    "ffx_frameinterpolation_reconstruct_previous_depth_pass"
    "ffx_frameinterpolation_setup_pass"
    "ffx_frameinterpolation_game_motion_vector_field_pass"
    "ffx_frameinterpolation_optical_flow_vector_field_pass"
    "ffx_frameinterpolation_pass"
    "ffx_frameinterpolation_compute_game_vector_field_inpainting_pyramid_pass"
    "ffx_frameinterpolation_compute_inpainting_pyramid_pass"
    "ffx_frameinterpolation_inpainting_pass"
    "ffx_frameinterpolation_debug_view_pass"
)

FI_ARGS="$BASE_ARGS -DFFX_FRAMEINTERPOLATION_EMBED_ROOTSIG=0"
FI_ARGS="$FI_ARGS -DFFX_FRAMEINTERPOLATION_OPTION_UPSAMPLE_SAMPLERS_USE_DATA_HALF=0"
FI_ARGS="$FI_ARGS -DFFX_FRAMEINTERPOLATION_OPTION_ACCUMULATE_SAMPLERS_USE_DATA_HALF=0"
FI_ARGS="$FI_ARGS -DFFX_FRAMEINTERPOLATION_OPTION_REPROJECT_SAMPLERS_USE_DATA_HALF=1"
FI_ARGS="$FI_ARGS -DFFX_FRAMEINTERPOLATION_OPTION_POSTPROCESSLOCKSTATUS_SAMPLERS_USE_DATA_HALF=0"
FI_ARGS="$FI_ARGS -DFFX_FRAMEINTERPOLATION_OPTION_UPSAMPLE_USE_LANCZOS_TYPE=2"

# Optical flow shader list
OF_SHADERS=(
    "ffx_opticalflow_compute_scd_divergence_pass"
    "ffx_opticalflow_scale_optical_flow_advanced_pass_v5"
    "ffx_opticalflow_filter_optical_flow_pass_v5"
    "ffx_opticalflow_compute_optical_flow_advanced_pass_v5"
    "ffx_opticalflow_prepare_luma_pass"
    "ffx_opticalflow_compute_luminance_pyramid_pass"
    "ffx_opticalflow_generate_scd_histogram_pass"
)

OF_ARGS="$BASE_ARGS -DFFX_OPTICALFLOW_EMBED_ROOTSIG=0"

compile_shader() {
    local shader_file="$1"
    local shader_name="$2"
    local extra_args="$3"
    local output_name="$4"

    echo "  Compiling: $output_name"
    "$DXC" -E CS -T cs_6_2 $SPIRV_ARGS $INCLUDE_ARGS $extra_args \
        -Fo "$OUTPUT_DIR/${output_name}.spv" \
        "$shader_file" 2>/dev/null || echo "    WARNING: Failed to compile $output_name"
}

echo "=== Building Frame Interpolation Shaders (SPIR-V) ==="
for shader in "${FI_SHADERS[@]}"; do
    shader_file="$SHADER_DIR/${shader}.hlsl"
    if [ ! -f "$shader_file" ]; then
        echo "  SKIP (not found): $shader"
        continue
    fi

    # Default permutation (no special options)
    compile_shader "$shader_file" "$shader" "$FI_ARGS -DFFX_HALF=0 -DFFX_FRAMEINTERPOLATION_OPTION_LOW_RES_MOTION_VECTORS=0 -DFFX_FRAMEINTERPOLATION_OPTION_JITTER_MOTION_VECTORS=0 -DFFX_FRAMEINTERPOLATION_OPTION_INVERTED_DEPTH=0" "${shader}"

    # 16-bit variant
    compile_shader "$shader_file" "$shader" "$FI_ARGS -DFFX_HALF=1 -enable-16bit-types -DFFX_FRAMEINTERPOLATION_OPTION_LOW_RES_MOTION_VECTORS=0 -DFFX_FRAMEINTERPOLATION_OPTION_JITTER_MOTION_VECTORS=0 -DFFX_FRAMEINTERPOLATION_OPTION_INVERTED_DEPTH=0" "${shader}_16bit"
done

echo ""
echo "=== Building Optical Flow Shaders (SPIR-V) ==="
for shader in "${OF_SHADERS[@]}"; do
    shader_file="$SHADER_DIR/${shader}.hlsl"
    if [ ! -f "$shader_file" ]; then
        echo "  SKIP (not found): $shader"
        continue
    fi

    compile_shader "$shader_file" "$shader" "$OF_ARGS -DFFX_HALF=0 -DFFX_OPTICALFLOW_OPTION_HDR_COLOR_INPUT=0" "${shader}"
    compile_shader "$shader_file" "$shader" "$OF_ARGS -DFFX_HALF=0 -DFFX_OPTICALFLOW_OPTION_HDR_COLOR_INPUT=1" "${shader}_hdr"
done

echo ""
echo "=== Done. SPIR-V outputs in: $OUTPUT_DIR ==="
