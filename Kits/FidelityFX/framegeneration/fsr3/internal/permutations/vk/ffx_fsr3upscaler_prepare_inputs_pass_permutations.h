#include "ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523.h"
#include "ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2.h"

typedef union ffx_fsr3upscaler_prepare_inputs_pass_PermutationKey {
    struct {
        uint32_t FFX_FSR3UPSCALER_OPTION_REPROJECT_USE_LANCZOS_TYPE : 1;
        uint32_t FFX_FSR3UPSCALER_OPTION_HDR_COLOR_INPUT : 1;
        uint32_t FFX_FSR3UPSCALER_OPTION_LOW_RESOLUTION_MOTION_VECTORS : 1;
        uint32_t FFX_FSR3UPSCALER_OPTION_JITTERED_MOTION_VECTORS : 1;
        uint32_t FFX_FSR3UPSCALER_OPTION_INVERTED_DEPTH : 1;
        uint32_t FFX_FSR3UPSCALER_OPTION_APPLY_SHARPENING : 1;
    };
    uint32_t index;
} ffx_fsr3upscaler_prepare_inputs_pass_PermutationKey;

typedef struct ffx_fsr3upscaler_prepare_inputs_pass_PermutationInfo {
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
} ffx_fsr3upscaler_prepare_inputs_pass_PermutationInfo;

static const uint32_t g_ffx_fsr3upscaler_prepare_inputs_pass_IndirectionTable[] = {
    1,
    1,
    1,
    1,
    4,
    4,
    4,
    4,
    3,
    3,
    3,
    3,
    5,
    5,
    5,
    5,
    0,
    0,
    0,
    0,
    6,
    6,
    6,
    6,
    2,
    2,
    2,
    2,
    7,
    7,
    7,
    7,
    1,
    1,
    1,
    1,
    4,
    4,
    4,
    4,
    3,
    3,
    3,
    3,
    5,
    5,
    5,
    5,
    0,
    0,
    0,
    0,
    6,
    6,
    6,
    6,
    2,
    2,
    2,
    2,
    7,
    7,
    7,
    7,
};

static const ffx_fsr3upscaler_prepare_inputs_pass_PermutationInfo g_ffx_fsr3upscaler_prepare_inputs_pass_PermutationInfo[] = {
    { g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_size, g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_73e636fdf5ad5f2a_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_size, g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_a14499875723befe_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_size, g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_1b090353d9d52dfd_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_size, g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_8b5807937609bcef_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_size, g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_2ea6b297c49475c2_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_size, g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_addbc88c68a7d640_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_size, g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_c46e81f7e80c6523_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_size, g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_data, 1, g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_CBVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_CBVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_CBVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_CBVResourceSpaces, 3, g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_TextureSRVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_TextureSRVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_TextureSRVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_TextureSRVResourceSpaces, 5, g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_TextureUAVResourceNames, g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_TextureUAVResourceBindings, g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_TextureUAVResourceCounts, g_ffx_fsr3upscaler_prepare_inputs_pass_8e5bcb88d7154db2_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

