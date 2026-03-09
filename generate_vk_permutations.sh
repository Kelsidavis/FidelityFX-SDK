#!/bin/bash
# Generate VK SPIR-V shader permutation headers using ffx_sc.py
# Works on Linux and macOS.
#
# Produces the permutation headers that ffx_frameinterpolation_shaderblobs.cpp
# and ffx_opticalflow_shaderblobs.cpp #include at compile time.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SDK_DIR="$SCRIPT_DIR/Kits/FidelityFX"
SHADER_DIR="$SDK_DIR/framegeneration/fsr3/internal/shaders"
OUTPUT_DIR="$SDK_DIR/framegeneration/fsr3/internal/permutations/vk"
FFX_SC="$SDK_DIR/tools/ffx_sc/ffx_sc.py"

# Auto-detect DXC: user override > Vulkan SDK > Homebrew > /tmp/dxc > PATH
if [ -z "$DXC" ]; then
    if [ -n "$VULKAN_SDK" ] && [ -f "$VULKAN_SDK/bin/dxc" ]; then
        DXC="$VULKAN_SDK/bin/dxc"
    elif [ -f "/opt/homebrew/bin/dxc" ]; then
        DXC="/opt/homebrew/bin/dxc"
    elif [ -f "/usr/local/bin/dxc" ]; then
        DXC="/usr/local/bin/dxc"
    elif [ -f "/tmp/dxc/bin/dxc" ]; then
        DXC="/tmp/dxc/bin/dxc"
    else
        DXC="dxc"
    fi
fi
export DXC

if ! command -v "$DXC" >/dev/null 2>&1 && [ ! -f "$DXC" ]; then
    echo "ERROR: DXC not found. Set DXC=/path/to/dxc"
    echo "  Install via: Vulkan SDK, Homebrew (brew install dxc), or"
    echo "  download from https://github.com/microsoft/DirectXShaderCompiler/releases"
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

INCLUDE_ARGS="-I $SDK_DIR/api/internal/gpu -I $SDK_DIR/framegeneration/fsr3/include/gpu"
BASE_ARGS="-reflection -embed-arguments -E CS -Wno-for-redefinition -Wno-ambig-lit-shift -DFFX_GPU=1 -DFFX_HLSL=1 -DFFX_IMPLICIT_SHADER_REGISTER_BINDING_HLSL=0"

compile_shader() {
    local shader_file="$1"
    local shader_name="$2"
    local profile="$3"
    shift 3
    # Remaining args are defines

    echo "  Compiling: $shader_name ($profile)"
    python3 "$FFX_SC" $BASE_ARGS "$@" \
        -T "$profile" -DFFX_HLSL_SM=${profile#cs_6_} \
        $INCLUDE_ARGS \
        -name="$shader_name" \
        -output="$OUTPUT_DIR" \
        "$shader_file" 2>&1 || echo "    WARNING: Failed $shader_name"
}

# ============================================================================
# Frame Interpolation Shaders
# ============================================================================
FI_ARGS="-DFFX_FRAMEINTERPOLATION_EMBED_ROOTSIG=0 -DFFX_FRAMEINTERPOLATION_OPTION_UPSAMPLE_SAMPLERS_USE_DATA_HALF=0 -DFFX_FRAMEINTERPOLATION_OPTION_ACCUMULATE_SAMPLERS_USE_DATA_HALF=0 -DFFX_FRAMEINTERPOLATION_OPTION_REPROJECT_SAMPLERS_USE_DATA_HALF=1 -DFFX_FRAMEINTERPOLATION_OPTION_POSTPROCESSLOCKSTATUS_SAMPLERS_USE_DATA_HALF=0 -DFFX_FRAMEINTERPOLATION_OPTION_UPSAMPLE_USE_LANCZOS_TYPE=2"
FI_PERM_ARGS="-DFFX_FRAMEINTERPOLATION_OPTION_LOW_RES_MOTION_VECTORS={0,1} -DFFX_FRAMEINTERPOLATION_OPTION_JITTER_MOTION_VECTORS={0,1} -DFFX_FRAMEINTERPOLATION_OPTION_INVERTED_DEPTH={0,1}"

FI_SHADERS=(
    ffx_frameinterpolation_reconstruct_and_dilate_pass
    ffx_frameinterpolation_disocclusion_mask_pass
    ffx_frameinterpolation_reconstruct_previous_depth_pass
    ffx_frameinterpolation_setup_pass
    ffx_frameinterpolation_game_motion_vector_field_pass
    ffx_frameinterpolation_optical_flow_vector_field_pass
    ffx_frameinterpolation_pass
    ffx_frameinterpolation_compute_game_vector_field_inpainting_pyramid_pass
    ffx_frameinterpolation_compute_inpainting_pyramid_pass
    ffx_frameinterpolation_inpainting_pass
    ffx_frameinterpolation_debug_view_pass
)

echo "=== Building Frame Interpolation Shaders (SPIR-V) ==="
for shader in "${FI_SHADERS[@]}"; do
    shader_file="$SHADER_DIR/${shader}.hlsl"
    [ ! -f "$shader_file" ] && echo "  SKIP: $shader" && continue

    # Wave32 (cs_6_2)
    compile_shader "$shader_file" "${shader}" cs_6_2 \
        $FI_ARGS $FI_PERM_ARGS -DFFX_HALF=0

    # Wave64 (cs_6_6)
    compile_shader "$shader_file" "${shader}_wave64" cs_6_6 \
        $FI_ARGS $FI_PERM_ARGS -DFFX_HALF=0 '-DFFX_PREFER_WAVE64=[WaveSize(64)]'

    # 16-bit (cs_6_2)
    compile_shader "$shader_file" "${shader}_16bit" cs_6_2 \
        $FI_ARGS $FI_PERM_ARGS -DFFX_HALF=1 -enable-16bit-types

    # Wave64 + 16-bit (cs_6_6)
    compile_shader "$shader_file" "${shader}_wave64_16bit" cs_6_6 \
        $FI_ARGS $FI_PERM_ARGS -DFFX_HALF=1 -enable-16bit-types '-DFFX_PREFER_WAVE64=[WaveSize(64)]'
done

# ============================================================================
# Optical Flow Shaders
# ============================================================================
OF_ARGS="-DFFX_OPTICALFLOW_EMBED_ROOTSIG=0"
OF_PERM_ARGS="-DFFX_OPTICALFLOW_OPTION_HDR_COLOR_INPUT={0,1}"

OF_SHADERS=(
    ffx_opticalflow_compute_scd_divergence_pass
    ffx_opticalflow_scale_optical_flow_advanced_pass_v5
    ffx_opticalflow_filter_optical_flow_pass_v5
    ffx_opticalflow_compute_optical_flow_advanced_pass_v5
    ffx_opticalflow_prepare_luma_pass
    ffx_opticalflow_compute_luminance_pyramid_pass
    ffx_opticalflow_generate_scd_histogram_pass
)

echo ""
echo "=== Building Optical Flow Shaders (SPIR-V) ==="
for shader in "${OF_SHADERS[@]}"; do
    shader_file="$SHADER_DIR/${shader}.hlsl"
    [ ! -f "$shader_file" ] && echo "  SKIP: $shader" && continue

    compile_shader "$shader_file" "${shader}" cs_6_2 \
        $OF_ARGS $OF_PERM_ARGS -DFFX_HALF=0

    compile_shader "$shader_file" "${shader}_wave64" cs_6_6 \
        $OF_ARGS $OF_PERM_ARGS -DFFX_HALF=0 '-DFFX_PREFER_WAVE64=[WaveSize(64)]'

    compile_shader "$shader_file" "${shader}_16bit" cs_6_2 \
        $OF_ARGS $OF_PERM_ARGS -DFFX_HALF=1 -enable-16bit-types

    compile_shader "$shader_file" "${shader}_wave64_16bit" cs_6_6 \
        $OF_ARGS $OF_PERM_ARGS -DFFX_HALF=1 -enable-16bit-types '-DFFX_PREFER_WAVE64=[WaveSize(64)]'
done

echo ""
echo "=== Done ==="
echo "Headers in: $OUTPUT_DIR"
echo "Total files: $(ls -1 "$OUTPUT_DIR"/*.h 2>/dev/null | wc -l)"
