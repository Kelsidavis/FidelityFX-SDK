// AmdExtD3DShaderIntrinsicsMatrixOps.hlsl
// Scalar compute shim for AMD WMMA intrinsics — Vulkan/SPIR-V port
// Emulates 16x16 wave matrix multiply-accumulate using wave shuffle intrinsics.
//
// Distribution: thread t (lane 0-31) holds 8 elements at flat indices [t*8 .. t*8+7]
// where flat = row * 16 + col for the 16x16 matrix.
// Thread t → row = t/2, col_start = (t%2)*8, cols = [col_start, col_start+7]
//
// Copyright (C) 2025 — MIT License (shim layer only)

#ifndef AMD_EXT_D3D_SHADER_INTRINSICS_MATRIX_OPS_HLSL
#define AMD_EXT_D3D_SHADER_INTRINSICS_MATRIX_OPS_HLSL

// ============================================================================
// Data format enum constants
// ============================================================================
static const uint AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F16 = 0;
static const uint AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F32 = 1;
static const uint AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_FP8 = 2;
static const uint AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_I8  = 3;
static const uint AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_I32 = 4;

// ============================================================================
// FP8 E4M3FN conversion helpers
// ============================================================================
float _wmma_fp8_to_float(uint bits8)
{
    uint sign = (bits8 >> 7) & 1;
    uint exp  = (bits8 >> 3) & 0xF;
    uint mant = bits8 & 0x7;

    if (exp == 0)
    {
        if (mant == 0) return sign ? -0.0f : 0.0f;
        // Subnormal: value = (-1)^s * mant/8 * 2^(-6)
        float val = float(mant) * (1.0f / 8.0f) * exp2(-6.0f);
        return sign ? -val : val;
    }
    if (exp == 15 && mant == 7)
        return 0.0f; // NaN sentinel → zero for inference

    float val = (1.0f + float(mant) / 8.0f) * exp2(float(exp) - 7.0f);
    return sign ? -val : val;
}

uint _wmma_float_to_fp8_sat(float v)
{
    if (isnan(v)) return 0x7F; // NaN
    uint sign = (v < 0.0f) ? 1u : 0u;
    v = abs(v);
    if (v > 448.0f) v = 448.0f; // clamp to E4M3 max
    if (v < 1.953125e-3f) return sign << 7; // underflow → signed zero

    uint fbits = asuint(v);
    int fexp = int((fbits >> 23) & 0xFF) - 127;
    uint fmant = fbits & 0x7FFFFF;

    int fp8_exp = fexp + 7;

    if (fp8_exp <= 0)
    {
        // Subnormal
        uint shift = uint(1 - fp8_exp);
        uint m = (8u | (fmant >> 20));
        uint fp8_mant = m >> shift;
        if (shift > 0 && ((m >> (shift - 1)) & 1)) fp8_mant++;
        if (fp8_mant > 7) { fp8_mant = 0; fp8_exp = 1; }
        return (sign << 7) | (fp8_mant & 0x7);
    }

    if (fp8_exp >= 15)
        return (sign << 7) | (15u << 3) | 6u; // max representable (avoid NaN at mant=7)

    uint fp8_mant = (fmant >> 20) & 0x7;
    if ((fmant >> 19) & 1)
    {
        fp8_mant++;
        if (fp8_mant > 7) { fp8_mant = 0; fp8_exp++; }
        if (fp8_exp >= 15) return (sign << 7) | (15u << 3) | 6u;
    }
    // Guard against NaN encoding
    if (fp8_exp == 15 && fp8_mant == 7) fp8_mant = 6;
    return (sign << 7) | (uint(fp8_exp) << 3) | fp8_mant;
}

// ============================================================================
// Generic element pack/unpack helpers
// ============================================================================

// Elements per uint for a given DataFormat
uint _wmma_epu(uint fmt)
{
    if (fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F32 ||
        fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_I32) return 1;
    if (fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F16) return 2;
    return 4; // FP8, I8
}

// Number of container r[] uints used for 8 elements
uint _wmma_regs(uint fmt)
{
    if (fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F32 ||
        fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_I32) return 8;
    if (fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F16) return 4;
    return 2; // FP8, I8
}

// Unpack one element from a packed uint
float _wmma_unpack(uint packed, uint sub, uint fmt)
{
    if (fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F32)
        return asfloat(packed);
    if (fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_I32)
        return float(asint(packed));
    if (fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F16)
        return f16tof32((packed >> (sub * 16)) & 0xFFFF);
    if (fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_FP8)
        return _wmma_fp8_to_float((packed >> (sub * 8)) & 0xFF);
    // I8 — signed
    int val = int((packed >> (sub * 8)) & 0xFF);
    if (val > 127) val -= 256;
    return float(val);
}

// Pack one element into a uint at sub-position (OR into existing bits)
uint _wmma_pack_bits(float val, uint sub, uint fmt)
{
    if (fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F32)
        return asuint(val);
    if (fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_I32)
        return uint(int(val));
    if (fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F16)
        return f32tof16(val) << (sub * 16);
    if (fmt == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_FP8)
        return _wmma_float_to_fp8_sat(val) << (sub * 8);
    // I8
    int iv = int(val);
    iv = clamp(iv, -128, 127);
    return (uint(iv) & 0xFF) << (sub * 8);
}

// ============================================================================
// Container struct
// ============================================================================
struct AmdWaveMatrixContainer
{
    uint r[8];
};

// ============================================================================
// Internal: get/set element as float from container with given DataFormat
// ============================================================================
float _wmma_getElem(AmdWaveMatrixContainer c, uint i, uint fmt)
{
    uint epu = _wmma_epu(fmt);
    return _wmma_unpack(c.r[i / epu], i % epu, fmt);
}

void _wmma_setElem(inout AmdWaveMatrixContainer c, uint i, float val, uint fmt)
{
    uint epu = _wmma_epu(fmt);
    uint reg = i / epu;
    uint sub = i % epu;
    if (epu == 1)
    {
        c.r[reg] = _wmma_pack_bits(val, 0, fmt);
    }
    else
    {
        // Clear sub-element bits, then OR in new value
        uint mask;
        if (epu == 2) mask = 0xFFFFu << (sub * 16);
        else          mask = 0xFFu << (sub * 8);
        c.r[reg] = (c.r[reg] & ~mask) | _wmma_pack_bits(val, sub, fmt);
    }
}

// ============================================================================
// Buffer load helpers — overloads for ByteAddressBuffer and RWByteAddressBuffer
// ============================================================================
uint _wmma_buf_load(ByteAddressBuffer b, uint a)   { return b.Load(a); }
uint _wmma_buf_load(RWByteAddressBuffer b, uint a)  { return b.Load(a); }
void _wmma_buf_store(RWByteAddressBuffer b, uint a, uint v) { b.Store(a, v); }

// ============================================================================
// AmdWaveMatrixA — input operand A (weights)
// ============================================================================
template<uint DataFormat, uint M, uint N>
struct AmdWaveMatrixA
{
    AmdWaveMatrixContainer container;

    uint Length() { return M * N / 32; } // 8 for 16x16

    float Element(uint i) { return _wmma_getElem(container, i, DataFormat); }

    void SetElement(uint i, float val) { _wmma_setElem(container, i, val, DataFormat); }

    void Fill(float val)
    {
        [unroll] for (uint i = 0; i < 8; i++)
            SetElement(i, val);
    }

    // Load from ByteAddressBuffer
    void Load(ByteAddressBuffer buf, uint offset, uint stride, bool colMajor)
    {
        _loadImpl(buf, offset, stride, colMajor);
    }
    // Load from RWByteAddressBuffer
    void Load(RWByteAddressBuffer buf, uint offset, uint stride, bool colMajor)
    {
        _loadImpl(buf, offset, stride, colMajor);
    }

    // Generic load implementation (works with either buffer type via overloaded _wmma_buf_load)
    template<typename BufType>
    void _loadImpl(BufType buf, uint offset, uint stride, bool colMajor)
    {
        uint lane = WaveGetLaneIndex();
        uint epu = _wmma_epu(DataFormat);
        uint regs = _wmma_regs(DataFormat);
        uint elemBytes = (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F32 ||
                          DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_I32) ? 4 :
                         (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F16) ? 2 : 1;

        // Load full packed uints from the buffer
        [unroll] for (uint ri = 0; ri < regs; ri++)
        {
            // Determine the flat start index for this packed uint
            uint elemStart = ri * epu;
            uint flat0 = lane * 8 + elemStart;
            uint row0 = flat0 / N;
            uint col0 = flat0 % N;

            uint addr;
            if (colMajor)
                addr = offset + col0 * stride + row0 * elemBytes;
            else
                addr = offset + row0 * stride + col0 * elemBytes;

            container.r[ri] = _wmma_buf_load(buf, addr);
        }
    }
};

// Forward declaration for cross-references
template<uint DataFormat, uint M, uint N> struct AmdWaveMatrixAccumulator;

// ============================================================================
// AmdWaveMatrixB — input operand B (activations)
// ============================================================================
template<uint DataFormat, uint M, uint N>
struct AmdWaveMatrixB
{
    AmdWaveMatrixContainer container;

    uint Length() { return M * N / 32; }

    float Element(uint i) { return _wmma_getElem(container, i, DataFormat); }

    void SetElement(uint i, float val) { _wmma_setElem(container, i, val, DataFormat); }

    void Fill(float val)
    {
        [unroll] for (uint i = 0; i < 8; i++)
            SetElement(i, val);
    }

    void Load(ByteAddressBuffer buf, uint offset, uint stride, bool colMajor)
    {
        _loadImpl(buf, offset, stride, colMajor);
    }
    void Load(RWByteAddressBuffer buf, uint offset, uint stride, bool colMajor)
    {
        _loadImpl(buf, offset, stride, colMajor);
    }

    template<typename BufType>
    void _loadImpl(BufType buf, uint offset, uint stride, bool colMajor)
    {
        uint lane = WaveGetLaneIndex();
        uint epu = _wmma_epu(DataFormat);
        uint regs = _wmma_regs(DataFormat);
        uint elemBytes = (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F32 ||
                          DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_I32) ? 4 :
                         (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F16) ? 2 : 1;

        [unroll] for (uint ri = 0; ri < regs; ri++)
        {
            uint elemStart = ri * epu;
            uint flat0 = lane * 8 + elemStart;
            uint row0 = flat0 / N;
            uint col0 = flat0 % N;

            uint addr;
            if (colMajor)
                addr = offset + col0 * stride + row0 * elemBytes;
            else
                addr = offset + row0 * stride + col0 * elemBytes;

            container.r[ri] = _wmma_buf_load(buf, addr);
        }
    }

    // Copy from accumulator (used for: B = acc after CopySat)
    template<uint SrcDF>
    void Copy(AmdWaveMatrixA<SrcDF, M, N> src)
    {
        [unroll] for (uint i = 0; i < 8; i++)
            SetElement(i, src.Element(i));
    }

    // CopySat from accumulator — convert with saturation
    template<uint AccDF>
    void CopySat(AmdWaveMatrixAccumulator<AccDF, M, N> src)
    {
        [unroll] for (uint i = 0; i < 8; i++)
        {
            float val = src.Element(i);
            // Saturation is format-specific clamping
            if (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_FP8)
                val = clamp(val, -448.0f, 448.0f);
            else if (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_I8)
                val = clamp(val, -128.0f, 127.0f);
            else if (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F16)
                val = clamp(val, -65504.0f, 65504.0f);
            SetElement(i, val);
        }
    }

    void Store(RWByteAddressBuffer buf, uint offset, uint stride, bool colMajor)
    {
        uint lane = WaveGetLaneIndex();
        uint epu = _wmma_epu(DataFormat);
        uint regs = _wmma_regs(DataFormat);
        uint elemBytes = (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F32 ||
                          DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_I32) ? 4 :
                         (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F16) ? 2 : 1;

        [unroll] for (uint ri = 0; ri < regs; ri++)
        {
            uint elemStart = ri * epu;
            uint flat0 = lane * 8 + elemStart;
            uint row0 = flat0 / N;
            uint col0 = flat0 % N;

            uint addr;
            if (colMajor)
                addr = offset + col0 * stride + row0 * elemBytes;
            else
                addr = offset + row0 * stride + col0 * elemBytes;

            _wmma_buf_store(buf, addr, container.r[ri]);
        }
    }
};

// ============================================================================
// AmdWaveMatrixAccumulator — output / accumulator matrix
// ============================================================================
template<uint DataFormat, uint M, uint N>
struct AmdWaveMatrixAccumulator
{
    AmdWaveMatrixContainer container;

    uint Length() { return M * N / 32; }

    float Element(uint i) { return _wmma_getElem(container, i, DataFormat); }

    void SetElement(uint i, float val) { _wmma_setElem(container, i, val, DataFormat); }

    void Fill(float val)
    {
        [unroll] for (uint i = 0; i < 8; i++)
            SetElement(i, val);
    }

    void Load(ByteAddressBuffer buf, uint offset, uint stride, bool colMajor)
    {
        _loadImpl(buf, offset, stride, colMajor);
    }
    void Load(RWByteAddressBuffer buf, uint offset, uint stride, bool colMajor)
    {
        _loadImpl(buf, offset, stride, colMajor);
    }

    template<typename BufType>
    void _loadImpl(BufType buf, uint offset, uint stride, bool colMajor)
    {
        uint lane = WaveGetLaneIndex();
        uint epu = _wmma_epu(DataFormat);
        uint regs = _wmma_regs(DataFormat);
        uint elemBytes = (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F32 ||
                          DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_I32) ? 4 :
                         (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F16) ? 2 : 1;

        [unroll] for (uint ri = 0; ri < regs; ri++)
        {
            uint elemStart = ri * epu;
            uint flat0 = lane * 8 + elemStart;
            uint row0 = flat0 / N;
            uint col0 = flat0 % N;

            uint addr;
            if (colMajor)
                addr = offset + col0 * stride + row0 * elemBytes;
            else
                addr = offset + row0 * stride + col0 * elemBytes;

            container.r[ri] = _wmma_buf_load(buf, addr);
        }
    }

    void Store(RWByteAddressBuffer buf, uint offset, uint stride, bool colMajor)
    {
        uint lane = WaveGetLaneIndex();
        uint epu = _wmma_epu(DataFormat);
        uint regs = _wmma_regs(DataFormat);
        uint elemBytes = (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F32 ||
                          DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_I32) ? 4 :
                         (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F16) ? 2 : 1;

        [unroll] for (uint ri = 0; ri < regs; ri++)
        {
            uint elemStart = ri * epu;
            uint flat0 = lane * 8 + elemStart;
            uint row0 = flat0 / N;
            uint col0 = flat0 % N;

            uint addr;
            if (colMajor)
                addr = offset + col0 * stride + row0 * elemBytes;
            else
                addr = offset + row0 * stride + col0 * elemBytes;

            _wmma_buf_store(buf, addr, container.r[ri]);
        }
    }

    // Copy from another accumulator (possibly different format)
    template<uint SrcDF>
    void Copy(AmdWaveMatrixAccumulator<SrcDF, M, N> src)
    {
        [unroll] for (uint i = 0; i < 8; i++)
            SetElement(i, src.Element(i));
    }

    // CopySat from another accumulator — convert with saturation
    template<uint SrcDF>
    void CopySat(AmdWaveMatrixAccumulator<SrcDF, M, N> src)
    {
        [unroll] for (uint i = 0; i < 8; i++)
        {
            float val = src.Element(i);
            if (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_FP8)
                val = clamp(val, -448.0f, 448.0f);
            else if (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_I8)
                val = clamp(val, -128.0f, 127.0f);
            else if (DataFormat == AmdExtD3DShaderIntrinsicsWaveMatrixDataFormat_F16)
                val = clamp(val, -65504.0f, 65504.0f);
            SetElement(i, val);
        }
    }

    // Copy from B matrix (used in residual connection paths)
    template<uint SrcDF>
    void Copy(AmdWaveMatrixB<SrcDF, M, N> src)
    {
        [unroll] for (uint i = 0; i < 8; i++)
            SetElement(i, src.Element(i));
    }
};

// ============================================================================
// AmdWaveMatrixMultiply — C = A * B + C
//
// Uses groupshared memory to exchange matrix elements across the workgroup.
// All 32 threads write their elements to shared arrays, barrier, then each
// thread reads the rows/columns it needs for its 8 output elements.
//
// Distribution: thread t holds flat indices [t*8..t*8+7]
//   → row = t/2, col_start = (t%2)*8
// ============================================================================

// Shared memory for matmul operands — 16x16 floats each = 256 floats = 1024 bytes each
groupshared float _wmma_shared_A[256];
groupshared float _wmma_shared_B[256];

// General matmul for any input format pair producing accumulator
template<uint DFA, uint DFB, uint DFC, uint M, uint K, uint N>
AmdWaveMatrixAccumulator<DFC, M, N> AmdWaveMatrixMultiply(
    AmdWaveMatrixA<DFA, M, K> A,
    AmdWaveMatrixB<DFB, K, N> B,
    AmdWaveMatrixAccumulator<DFC, M, N> Acc)
{
    uint lane = WaveGetLaneIndex();

    // ---- Scatter A and B elements to shared memory ----
    // Each thread writes its 8 elements at flat positions [lane*8..lane*8+7]
    [unroll] for (uint i = 0; i < 8; i++)
    {
        _wmma_shared_A[lane * 8 + i] = _wmma_getElem(A.container, i, DFA);
        _wmma_shared_B[lane * 8 + i] = _wmma_getElem(B.container, i, DFB);
    }
    GroupMemoryBarrierWithGroupSync();

    // ---- Compute: each thread's 8 output elements ----
    uint row = lane / 2;
    uint col_start = (lane % 2) * 8;

    [unroll] for (uint i = 0; i < 8; i++)
    {
        float sum = Acc.Element(i);
        uint col = col_start + i;
        [unroll] for (uint k = 0; k < 16; k++)
            sum += _wmma_shared_A[row * 16 + k] * _wmma_shared_B[k * 16 + col];
        Acc.SetElement(i, sum);
    }

    GroupMemoryBarrierWithGroupSync();
    return Acc;
}

// ============================================================================
// AMD_GROUPSHARED_LOAD
//
// Loads matrix data from a groupshared uint array.
// The stride parameter (in uints) implicitly encodes the format:
//   stride 4  → 4 elements/uint (FP8/I8)
//   stride 8  → 2 elements/uint (F16)
//   stride 16 → 1 element/uint  (F32/I32)
// ============================================================================
#define AMD_GROUPSHARED_LOAD(mat, gs, gs_offset, gs_stride, is_col_major) \
{ \
    uint _wml_lane = WaveGetLaneIndex(); \
    uint _wml_epu = 16u / (gs_stride); \
    [unroll] for (uint _wml_i = 0; _wml_i < 8; _wml_i++) \
    { \
        uint _wml_flat = _wml_lane * 8 + _wml_i; \
        uint _wml_row = _wml_flat / 16; \
        uint _wml_col = _wml_flat % 16; \
        uint _wml_ric = (is_col_major) ? _wml_row : _wml_col; \
        uint _wml_ci  = (is_col_major) ? _wml_col : _wml_row; \
        uint _wml_gsi = (gs_offset) + _wml_ci * (gs_stride) + _wml_ric / _wml_epu; \
        uint _wml_sub = _wml_ric % _wml_epu; \
        uint _wml_raw = (gs)[_wml_gsi]; \
        float _wml_val; \
        if (_wml_epu == 1u) { _wml_val = asfloat(_wml_raw); } \
        else if (_wml_epu == 2u) { _wml_val = f16tof32((_wml_raw >> (_wml_sub * 16u)) & 0xFFFFu); } \
        else { _wml_val = _wmma_fp8_to_float((_wml_raw >> (_wml_sub * 8u)) & 0xFFu); } \
        (mat).SetElement(_wml_i, _wml_val); \
    } \
}

// ============================================================================
// AMD_GROUPSHARED_STORE
//
// Stores matrix data to a groupshared uint array.
// For multi-element-per-uint formats, uses InterlockedOr after clearing.
// ============================================================================
#define AMD_GROUPSHARED_STORE(mat, gs, gs_offset, gs_stride, is_col_major) \
{ \
    uint _wms_lane = WaveGetLaneIndex(); \
    uint _wms_epu = 16u / (gs_stride); \
    if (_wms_epu > 1u) \
    { \
        uint _wms_total = 16u * (gs_stride); \
        [unroll] for (uint _wms_c = _wms_lane; _wms_c < _wms_total; _wms_c += 32u) \
            (gs)[(gs_offset) + _wms_c] = 0u; \
        GroupMemoryBarrierWithGroupSync(); \
    } \
    [unroll] for (uint _wms_i = 0; _wms_i < 8; _wms_i++) \
    { \
        uint _wms_flat = _wms_lane * 8 + _wms_i; \
        uint _wms_row = _wms_flat / 16; \
        uint _wms_col = _wms_flat % 16; \
        uint _wms_ric = (is_col_major) ? _wms_row : _wms_col; \
        uint _wms_ci  = (is_col_major) ? _wms_col : _wms_row; \
        uint _wms_gsi = (gs_offset) + _wms_ci * (gs_stride) + _wms_ric / _wms_epu; \
        float _wms_val = (mat).Element(_wms_i); \
        if (_wms_epu == 1u) \
        { \
            (gs)[_wms_gsi] = asuint(_wms_val); \
        } \
        else if (_wms_epu == 2u) \
        { \
            uint _wms_sub = _wms_ric % 2u; \
            InterlockedOr((gs)[_wms_gsi], f32tof16(_wms_val) << (_wms_sub * 16u)); \
        } \
        else \
        { \
            uint _wms_sub = _wms_ric % 4u; \
            InterlockedOr((gs)[_wms_gsi], _wmma_float_to_fp8_sat(_wms_val) << (_wms_sub * 8u)); \
        } \
    } \
    if (_wms_epu > 1u) GroupMemoryBarrierWithGroupSync(); \
}

#endif // AMD_EXT_D3D_SHADER_INTRINSICS_MATRIX_OPS_HLSL
