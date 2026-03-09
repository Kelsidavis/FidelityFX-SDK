#include "ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f.h"
#include "ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828.h"

typedef union ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_PermutationKey {
    struct {
        uint32_t FFX_FRAMEINTERPOLATION_OPTION_LOW_RES_MOTION_VECTORS : 1;
        uint32_t FFX_FRAMEINTERPOLATION_OPTION_JITTER_MOTION_VECTORS : 1;
        uint32_t FFX_FRAMEINTERPOLATION_OPTION_INVERTED_DEPTH : 1;
    };
    uint32_t index;
} ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_PermutationKey;

typedef struct ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_PermutationInfo {
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
} ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_PermutationInfo;

static const uint32_t g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_IndirectionTable[] = {
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
};

static const ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_PermutationInfo g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_PermutationInfo[] = {
    { g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_size, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_data, 1, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_CBVResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_CBVResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_CBVResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_CBVResourceSpaces, 7, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_TextureSRVResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_TextureSRVResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_TextureSRVResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_TextureSRVResourceSpaces, 1, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_TextureUAVResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_TextureUAVResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_TextureUAVResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_SamplerResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_SamplerResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_SamplerResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_451df5c0e261619f_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_size, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_data, 1, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_CBVResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_CBVResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_CBVResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_CBVResourceSpaces, 7, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_TextureSRVResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_TextureSRVResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_TextureSRVResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_TextureSRVResourceSpaces, 1, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_TextureUAVResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_TextureUAVResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_TextureUAVResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_SamplerResourceNames, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_SamplerResourceBindings, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_SamplerResourceCounts, g_ffx_frameinterpolation_disocclusion_mask_pass_wave64_16bit_c5f15ef48a000828_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
};

