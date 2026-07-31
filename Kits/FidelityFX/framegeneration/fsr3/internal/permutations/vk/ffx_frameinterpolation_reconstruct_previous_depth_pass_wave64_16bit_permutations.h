#include "ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a.h"
#include "ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa.h"

typedef union ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_PermutationKey {
    struct {
        uint32_t FFX_FRAMEINTERPOLATION_OPTION_LOW_RES_MOTION_VECTORS : 1;
        uint32_t FFX_FRAMEINTERPOLATION_OPTION_JITTER_MOTION_VECTORS : 1;
        uint32_t FFX_FRAMEINTERPOLATION_OPTION_INVERTED_DEPTH : 1;
    };
    uint32_t index;
} ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_PermutationKey;

typedef struct ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_PermutationInfo {
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
} ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_PermutationInfo;

static const uint32_t g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_IndirectionTable[] = {
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
};

static const ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_PermutationInfo g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_PermutationInfo[] = {
    { g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_size, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_data, 1, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_CBVResourceNames, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_CBVResourceBindings, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_CBVResourceCounts, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_CBVResourceSpaces, 3, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_TextureSRVResourceNames, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_TextureSRVResourceBindings, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_TextureSRVResourceCounts, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_TextureSRVResourceSpaces, 1, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_TextureUAVResourceNames, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_TextureUAVResourceBindings, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_TextureUAVResourceCounts, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_SamplerResourceNames, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_SamplerResourceBindings, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_SamplerResourceCounts, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_8d779497dea5d68a_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_size, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_data, 1, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_CBVResourceNames, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_CBVResourceBindings, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_CBVResourceCounts, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_CBVResourceSpaces, 3, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_TextureSRVResourceNames, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_TextureSRVResourceBindings, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_TextureSRVResourceCounts, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_TextureSRVResourceSpaces, 1, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_TextureUAVResourceNames, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_TextureUAVResourceBindings, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_TextureUAVResourceCounts, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_SamplerResourceNames, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_SamplerResourceBindings, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_SamplerResourceCounts, g_ffx_frameinterpolation_reconstruct_previous_depth_pass_wave64_16bit_ce8460ecf26330aa_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
};

