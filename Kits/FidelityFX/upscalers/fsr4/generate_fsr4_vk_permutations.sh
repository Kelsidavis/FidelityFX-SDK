#!/bin/bash
# Generate FSR4 VK SPIR-V shader permutation headers using ffx_sc.py
# Compiles all FSR4 ML upscaler shaders (pre, model passes 1-12, post, utilities)
# to SPIR-V and generates permutation headers with embedded bytecode + reflection.
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SDK_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
SHADER_DIR="$SCRIPT_DIR/internal/shaders"
OUTPUT_DIR="${1:-$SCRIPT_DIR/internal/permutations/vk}"
FFX_SC="$SDK_DIR/tools/ffx_sc/ffx_sc.py"

# Auto-detect DXC
if [ -z "$DXC" ]; then
    for try in "$VULKAN_SDK/bin/dxc" /opt/homebrew/bin/dxc /usr/local/bin/dxc /tmp/dxc/bin/dxc; do
        [ -f "$try" ] && DXC="$try" && break
    done
    [ -z "$DXC" ] && DXC="dxc"
fi
export DXC

if ! command -v "$DXC" >/dev/null 2>&1 && [ ! -f "$DXC" ]; then
    echo "ERROR: DXC not found. Set DXC=/path/to/dxc"
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

# Include paths for FSR4 shaders
INCLUDE_ARGS="-I $SCRIPT_DIR/include/gpu/fsr4 -I $SCRIPT_DIR/include/gpu -I $SCRIPT_DIR/dx12 -I $SDK_DIR/api/internal/gpu"

# Common args for ffx_sc.py
BASE_ARGS="-reflection -embed-arguments -Wno-for-redefinition -Wno-ambig-lit-shift -DFFX_GPU=1 -DFFX_HLSL=1"

# DATA_TYPE overrides for Vulkan (no float16_t textures)
VK_TYPE_ARGS="-DDATA_TYPE=float -DDATA_TYPE3=float3 -DDATA_TYPE_VECTOR=float4"

# WMMA shim enable
WMMA_ARGS="-DWMMA_ENABLED=1"

compile_shader() {
    local shader_file="$1"
    local shader_name="$2"
    local entry_point="$3"
    shift 3
    # Remaining args are defines/permutation args

    echo "  Compiling: $shader_name (entry: $entry_point)"
    python3 "$FFX_SC" $BASE_ARGS "$@" \
        -E "$entry_point" \
        -T cs_6_2 -DFFX_HLSL_SM=62 \
        $INCLUDE_ARGS \
        $VK_TYPE_ARGS \
        $WMMA_ARGS \
        -enable-16bit-types \
        -HV 2021 \
        -name="$shader_name" \
        -output="$OUTPUT_DIR" \
        "$shader_file" 2>&1 || echo "    WARNING: Failed $shader_name"
}

# ============================================================================
# Pre-shader (pass 0) — permutations for depth, MV, colorspace, etc.
# ============================================================================
echo "=== Building FSR4 Pre-shader (pass 0) ==="
PRE_PERM_ARGS="-DFFX_MLSR_DEPTH_INVERTED={0,1} -DFFX_MLSR_LOW_RES_MV={0,1} -DFFX_MLSR_AUTOEXPOSURE_ENABLED={0,1} -DFFX_MLSR_COLORSPACE={0,1,2,3} -DFFX_MLSR_JITTERED_MOTION_VECTORS={0,1} -DFFX_MLSR_RESOLUTION={0,1,2}"

compile_shader "$SHADER_DIR/pre_wmma.hlsl" \
    "fsr4_model_v07_fp8_no_scale_0" \
    "fsr4_model_v07_fp8_no_scale_prepass" \
    $PRE_PERM_ARGS

# ============================================================================
# Post-shader (pass 13) — permutations for colorspace, autoexposure, debug
# ============================================================================
echo ""
echo "=== Building FSR4 Post-shader (pass 13) ==="
POST_PERM_ARGS="-DFFX_MLSR_COLORSPACE={0,1,2,3} -DFFX_MLSR_AUTOEXPOSURE_ENABLED={0,1} -DFFX_DEBUG_VISUALIZE={0,1} -DFFX_MLSR_RESOLUTION={0,1,2}"

compile_shader "$SHADER_DIR/post_wmma.hlsl" \
    "fsr4_model_v07_fp8_no_scale_13" \
    "fsr4_model_v07_fp8_no_scale_postpass" \
    $POST_PERM_ARGS

# ============================================================================
# Model passes (1-12) — one permutation per resolution tier
# ============================================================================
echo ""
echo "=== Building FSR4 Model Passes ==="
RESOLUTIONS=(1080 2160 4320)

for res in "${RESOLUTIONS[@]}"; do
    shader_file="$SHADER_DIR/fsr4_model_v07_fp8_no_scale_passes_${res}.hlsl"
    [ ! -f "$shader_file" ] && echo "  SKIP: passes_${res} (not found)" && continue

    for pass in $(seq 1 12); do
        # Model pass
        compile_shader "$shader_file" \
            "fsr4_model_v07_fp8_no_scale_${res}_${pass}" \
            "fsr4_model_v07_fp8_no_scale_pass${pass}" \
            "-DMLSR_PASS_${pass}=1"

        # Padding reset (post) variant
        compile_shader "$shader_file" \
            "fsr4_model_v07_fp8_no_scale_${res}_${pass}_post" \
            "fsr4_model_v07_fp8_no_scale_pass${pass}_post" \
            "-DMLSR_PASS_${pass}_POST=1"
    done

    # Pass 0 post (padding reset for pre-pass output)
    compile_shader "$shader_file" \
        "fsr4_model_v07_fp8_no_scale_${res}_0_post" \
        "fsr4_model_v07_fp8_no_scale_pass0_post" \
        "-DMLSR_PASS_0_POST=1"
done

# ============================================================================
# Utility shaders — RCAS, SPD auto-exposure, debug view
# ============================================================================
echo ""
echo "=== Building FSR4 Utility Shaders ==="

# RCAS (sharpening)
if [ -f "$SHADER_DIR/rcas.hlsl" ]; then
    RCAS_PERM_ARGS="-DFFX_MLSR_COLORSPACE={0,1,2,3} -DFFX_MLSR_AUTOEXPOSURE_ENABLED={0,1}"
    compile_shader "$SHADER_DIR/rcas.hlsl" \
        "fsr4_rcas" \
        "CS" \
        $RCAS_PERM_ARGS \
        -DFFX_HALF=0
fi

# SPD auto-exposure
if [ -f "$SHADER_DIR/spd_auto_exposure.hlsl" ]; then
    compile_shader "$SHADER_DIR/spd_auto_exposure.hlsl" \
        "fsr4_spd_auto_exposure" \
        "CS" \
        -DFFX_HALF=0
fi

# Debug view
if [ -f "$SHADER_DIR/debug_view.hlsl" ]; then
    DEBUG_PERM_ARGS="-DFFX_MLSR_COLORSPACE={0,1,2,3} -DFFX_MLSR_AUTOEXPOSURE_ENABLED={0,1}"
    compile_shader "$SHADER_DIR/debug_view.hlsl" \
        "fsr4_debug_view" \
        "CS" \
        $DEBUG_PERM_ARGS \
        -DFFX_HALF=0
fi

echo ""
echo "=== Done ==="
echo "Headers in: $OUTPUT_DIR"
echo "Total files: $(ls -1 "$OUTPUT_DIR"/*.h 2>/dev/null | wc -l)"
