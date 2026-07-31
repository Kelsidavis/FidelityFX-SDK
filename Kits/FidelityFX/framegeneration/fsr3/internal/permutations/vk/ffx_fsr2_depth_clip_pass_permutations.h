#include "ffx_fsr2_depth_clip_pass_beab866fdbd4a995.h"
#include "ffx_fsr2_depth_clip_pass_cbaf4cdabf003842.h"
#include "ffx_fsr2_depth_clip_pass_52a35880c9f31dad.h"
#include "ffx_fsr2_depth_clip_pass_651eded77d575ac2.h"
#include "ffx_fsr2_depth_clip_pass_b2f2de9e1156f767.h"
#include "ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3.h"
#include "ffx_fsr2_depth_clip_pass_5bed88e23c3e429a.h"
#include "ffx_fsr2_depth_clip_pass_7e8eda914120ac94.h"

typedef union ffx_fsr2_depth_clip_pass_PermutationKey {
    struct {
        uint32_t FFX_FSR2_OPTION_REPROJECT_USE_LANCZOS_TYPE : 1;
        uint32_t FFX_FSR2_OPTION_HDR_COLOR_INPUT : 1;
        uint32_t FFX_FSR2_OPTION_LOW_RESOLUTION_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_JITTERED_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_INVERTED_DEPTH : 1;
        uint32_t FFX_FSR2_OPTION_APPLY_SHARPENING : 1;
    };
    uint32_t index;
} ffx_fsr2_depth_clip_pass_PermutationKey;

typedef struct ffx_fsr2_depth_clip_pass_PermutationInfo {
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
} ffx_fsr2_depth_clip_pass_PermutationInfo;

static const uint32_t g_ffx_fsr2_depth_clip_pass_IndirectionTable[] = {
    4,
    4,
    4,
    4,
    5,
    5,
    5,
    5,
    0,
    0,
    0,
    0,
    7,
    7,
    7,
    7,
    1,
    1,
    1,
    1,
    3,
    3,
    3,
    3,
    2,
    2,
    2,
    2,
    6,
    6,
    6,
    6,
    4,
    4,
    4,
    4,
    5,
    5,
    5,
    5,
    0,
    0,
    0,
    0,
    7,
    7,
    7,
    7,
    1,
    1,
    1,
    1,
    3,
    3,
    3,
    3,
    2,
    2,
    2,
    2,
    6,
    6,
    6,
    6,
};

static const ffx_fsr2_depth_clip_pass_PermutationInfo g_ffx_fsr2_depth_clip_pass_PermutationInfo[] = {
    { g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_size, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_data, 1, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_beab866fdbd4a995_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_size, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_data, 1, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_cbaf4cdabf003842_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_size, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_data, 1, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_52a35880c9f31dad_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_size, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_data, 1, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_651eded77d575ac2_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_size, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_data, 1, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_b2f2de9e1156f767_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_size, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_data, 1, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_96183f8cf86fa1a3_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_size, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_data, 1, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_5bed88e23c3e429a_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_size, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_data, 1, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_7e8eda914120ac94_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
};

