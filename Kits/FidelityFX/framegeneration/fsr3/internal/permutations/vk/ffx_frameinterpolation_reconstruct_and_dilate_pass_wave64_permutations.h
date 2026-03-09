#include "ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497.h"
#include "ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658.h"
#include "ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b.h"
#include "ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035.h"

typedef union ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_PermutationKey {
    struct {
        uint32_t FFX_FRAMEINTERPOLATION_OPTION_LOW_RES_MOTION_VECTORS : 1;
        uint32_t FFX_FRAMEINTERPOLATION_OPTION_JITTER_MOTION_VECTORS : 1;
        uint32_t FFX_FRAMEINTERPOLATION_OPTION_INVERTED_DEPTH : 1;
    };
    uint32_t index;
} ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_PermutationKey;

typedef struct ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_PermutationInfo {
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
} ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_PermutationInfo;

static const uint32_t g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_IndirectionTable[] = {
    0,
    2,
    0,
    2,
    1,
    3,
    1,
    3,
};

static const ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_PermutationInfo g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_PermutationInfo[] = {
    { g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_size, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_data, 1, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_CBVResourceNames, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_CBVResourceBindings, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_CBVResourceCounts, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_CBVResourceSpaces, 2, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_TextureSRVResourceNames, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_TextureSRVResourceBindings, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_TextureSRVResourceCounts, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_TextureSRVResourceSpaces, 3, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_TextureUAVResourceNames, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_TextureUAVResourceBindings, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_TextureUAVResourceCounts, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_fe40d66864adc497_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_size, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_data, 1, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_CBVResourceNames, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_CBVResourceBindings, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_CBVResourceCounts, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_CBVResourceSpaces, 2, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_TextureSRVResourceNames, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_TextureSRVResourceBindings, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_TextureSRVResourceCounts, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_TextureSRVResourceSpaces, 3, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_TextureUAVResourceNames, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_TextureUAVResourceBindings, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_TextureUAVResourceCounts, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_81a2bb095687c658_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_size, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_data, 1, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_CBVResourceNames, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_CBVResourceBindings, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_CBVResourceCounts, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_CBVResourceSpaces, 2, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_TextureSRVResourceNames, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_TextureSRVResourceBindings, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_TextureSRVResourceCounts, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_TextureSRVResourceSpaces, 3, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_TextureUAVResourceNames, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_TextureUAVResourceBindings, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_TextureUAVResourceCounts, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_f4cd8ec7f3c19f2b_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_size, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_data, 1, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_CBVResourceNames, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_CBVResourceBindings, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_CBVResourceCounts, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_CBVResourceSpaces, 2, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_TextureSRVResourceNames, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_TextureSRVResourceBindings, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_TextureSRVResourceCounts, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_TextureSRVResourceSpaces, 3, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_TextureUAVResourceNames, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_TextureUAVResourceBindings, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_TextureUAVResourceCounts, g_ffx_frameinterpolation_reconstruct_and_dilate_pass_wave64_69e5b59d38be0035_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

