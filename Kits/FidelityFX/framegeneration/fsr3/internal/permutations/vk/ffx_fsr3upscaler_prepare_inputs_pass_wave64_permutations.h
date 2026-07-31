#include "ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55.h"

typedef union ffx_fsr3upscaler_prepare_inputs_pass_wave64_PermutationKey {
    struct {
        uint32_t FFX_FSR3UPSCALER_OPTION_REPROJECT_USE_LANCZOS_TYPE : 1;
        uint32_t FFX_FSR3UPSCALER_OPTION_HDR_COLOR_INPUT : 1;
        uint32_t FFX_FSR3UPSCALER_OPTION_LOW_RESOLUTION_MOTION_VECTORS : 1;
        uint32_t FFX_FSR3UPSCALER_OPTION_JITTERED_MOTION_VECTORS : 1;
        uint32_t FFX_FSR3UPSCALER_OPTION_INVERTED_DEPTH : 1;
        uint32_t FFX_FSR3UPSCALER_OPTION_APPLY_SHARPENING : 1;
    };
    uint32_t index;
} ffx_fsr3upscaler_prepare_inputs_pass_wave64_PermutationKey;

typedef struct ffx_fsr3upscaler_prepare_inputs_pass_wave64_PermutationInfo {
    const uint32_t       blobSize;
    const unsigned char* blobData;


    const uint32_t  numConstantBuffers;
    const char**    constantBufferNames;
    const uint32_t* constantBufferBindings;
    const uint32_t* constantBufferCounts;
    const uint32_t* constantBufferSpaces;

    const uint32_t  numSRVTextures;
    const char**    srvTextureNames;
    const uint32_t* srvTextureBindings;
    const uint32_t* srvTextureCounts;
    const uint32_t* srvTextureSpaces;

    const uint32_t  numUAVTextures;
    const char**    uavTextureNames;
    const uint32_t* uavTextureBindings;
    const uint32_t* uavTextureCounts;
    const uint32_t* uavTextureSpaces;

    const uint32_t  numSRVBuffers;
    const char**    srvBufferNames;
    const uint32_t* srvBufferBindings;
    const uint32_t* srvBufferCounts;
    const uint32_t* srvBufferSpaces;

    const uint32_t  numUAVBuffers;
    const char**    uavBufferNames;
    const uint32_t* uavBufferBindings;
    const uint32_t* uavBufferCounts;
    const uint32_t* uavBufferSpaces;

    const uint32_t  numSamplers;
    const char**    samplerNames;
    const uint32_t* samplerBindings;
    const uint32_t* samplerCounts;
    const uint32_t* samplerSpaces;

    const uint32_t  numRTAccelerationStructures;
    const char**    rtAccelerationStructureNames;
    const uint32_t* rtAccelerationStructureBindings;
    const uint32_t* rtAccelerationStructureCounts;
    const uint32_t* rtAccelerationStructureSpaces;
} ffx_fsr3upscaler_prepare_inputs_pass_wave64_PermutationInfo;

static const uint32_t g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_IndirectionTable[] = {
    2,
    2,
    2,
    2,
    3,
    3,
    3,
    3,
    5,
    5,
    5,
    5,
    6,
    6,
    6,
    6,
    1,
    1,
    1,
    1,
    4,
    4,
    4,
    4,
    0,
    0,
    0,
    0,
    7,
    7,
    7,
    7,
    2,
    2,
    2,
    2,
    3,
    3,
    3,
    3,
    5,
    5,
    5,
    5,
    6,
    6,
    6,
    6,
    1,
    1,
    1,
    1,
    4,
    4,
    4,
    4,
    0,
    0,
    0,
    0,
    7,
    7,
    7,
    7,
};

static const ffx_fsr3upscaler_prepare_inputs_pass_wave64_PermutationInfo g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_PermutationInfo[] = {
    { g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_size, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e158e5c705d59655_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_size, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6de8d603cc5c8342_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_size, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_7a8e794ddeaedf15_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_size, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_229facaab83e3f1f_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_size, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_145e03b1c5c92f7f_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_size, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_39e2704a7b1dd539_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_size, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_6d45a0360925b280_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_size, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_wave64_e752072fc55a6f55_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

