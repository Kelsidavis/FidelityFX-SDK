#include "debug_view_f8aea96add17f530.h"
#include "debug_view_c8114c9db3792e31.h"

typedef union debug_view_PermutationKey {
    struct {
        uint32_t FFX_MLSR_COLORSPACE : 2;
        uint32_t FFX_MLSR_AUTOEXPOSURE_ENABLED : 1;
        uint32_t FFX_MLSR_JITTERED_MOTION_VECTORS : 1;
    };
    uint32_t index;
} debug_view_PermutationKey;

typedef struct debug_view_PermutationInfo {
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
} debug_view_PermutationInfo;

static const uint32_t g_debug_view_IndirectionTable[] = {
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

static const debug_view_PermutationInfo g_debug_view_PermutationInfo[] = {
    { g_debug_view_f8aea96add17f530_size, g_debug_view_f8aea96add17f530_data, 1, g_debug_view_f8aea96add17f530_CBVResourceNames, g_debug_view_f8aea96add17f530_CBVResourceBindings, g_debug_view_f8aea96add17f530_CBVResourceCounts, g_debug_view_f8aea96add17f530_CBVResourceSpaces, 2, g_debug_view_f8aea96add17f530_TextureSRVResourceNames, g_debug_view_f8aea96add17f530_TextureSRVResourceBindings, g_debug_view_f8aea96add17f530_TextureSRVResourceCounts, g_debug_view_f8aea96add17f530_TextureSRVResourceSpaces, 2, g_debug_view_f8aea96add17f530_TextureUAVResourceNames, g_debug_view_f8aea96add17f530_TextureUAVResourceBindings, g_debug_view_f8aea96add17f530_TextureUAVResourceCounts, g_debug_view_f8aea96add17f530_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_debug_view_f8aea96add17f530_SamplerResourceNames, g_debug_view_f8aea96add17f530_SamplerResourceBindings, g_debug_view_f8aea96add17f530_SamplerResourceCounts, g_debug_view_f8aea96add17f530_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_debug_view_c8114c9db3792e31_size, g_debug_view_c8114c9db3792e31_data, 1, g_debug_view_c8114c9db3792e31_CBVResourceNames, g_debug_view_c8114c9db3792e31_CBVResourceBindings, g_debug_view_c8114c9db3792e31_CBVResourceCounts, g_debug_view_c8114c9db3792e31_CBVResourceSpaces, 2, g_debug_view_c8114c9db3792e31_TextureSRVResourceNames, g_debug_view_c8114c9db3792e31_TextureSRVResourceBindings, g_debug_view_c8114c9db3792e31_TextureSRVResourceCounts, g_debug_view_c8114c9db3792e31_TextureSRVResourceSpaces, 2, g_debug_view_c8114c9db3792e31_TextureUAVResourceNames, g_debug_view_c8114c9db3792e31_TextureUAVResourceBindings, g_debug_view_c8114c9db3792e31_TextureUAVResourceCounts, g_debug_view_c8114c9db3792e31_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_debug_view_c8114c9db3792e31_SamplerResourceNames, g_debug_view_c8114c9db3792e31_SamplerResourceBindings, g_debug_view_c8114c9db3792e31_SamplerResourceCounts, g_debug_view_c8114c9db3792e31_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
};

