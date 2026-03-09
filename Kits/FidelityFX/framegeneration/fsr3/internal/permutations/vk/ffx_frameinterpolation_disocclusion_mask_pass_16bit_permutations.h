#include "ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123.h"
#include "ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db.h"

typedef union ffx_frameinterpolation_disocclusion_mask_pass_16bit_PermutationKey {
    struct {
        uint32_t FFX_FRAMEINTERPOLATION_OPTION_LOW_RES_MOTION_VECTORS : 1;
        uint32_t FFX_FRAMEINTERPOLATION_OPTION_JITTER_MOTION_VECTORS : 1;
        uint32_t FFX_FRAMEINTERPOLATION_OPTION_INVERTED_DEPTH : 1;
    };
    uint32_t index;
} ffx_frameinterpolation_disocclusion_mask_pass_16bit_PermutationKey;

typedef struct ffx_frameinterpolation_disocclusion_mask_pass_16bit_PermutationInfo {
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
} ffx_frameinterpolation_disocclusion_mask_pass_16bit_PermutationInfo;

static const uint32_t g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_IndirectionTable[] = {
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
};

static const ffx_frameinterpolation_disocclusion_mask_pass_16bit_PermutationInfo g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_PermutationInfo[] = {
    { g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_size, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_data, 1, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_CBVResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_CBVResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_CBVResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_CBVResourceSpaces, 7, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_TextureSRVResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_TextureSRVResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_TextureSRVResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_TextureSRVResourceSpaces, 1, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_TextureUAVResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_TextureUAVResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_TextureUAVResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_SamplerResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_SamplerResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_SamplerResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_68d27d42acc99123_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_size, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_data, 1, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_CBVResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_CBVResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_CBVResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_CBVResourceSpaces, 7, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_TextureSRVResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_TextureSRVResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_TextureSRVResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_TextureSRVResourceSpaces, 1, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_TextureUAVResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_TextureUAVResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_TextureUAVResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_SamplerResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_SamplerResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_SamplerResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_16bit_b02360a279f4b8db_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
};

