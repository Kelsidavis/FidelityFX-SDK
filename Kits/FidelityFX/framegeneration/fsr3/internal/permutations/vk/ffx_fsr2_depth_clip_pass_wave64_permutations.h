#include "ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703.h"
#include "ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2.h"
#include "ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0.h"
#include "ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52.h"
#include "ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed.h"
#include "ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee.h"
#include "ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34.h"
#include "ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8.h"

typedef union ffx_fsr2_depth_clip_pass_wave64_PermutationKey {
    struct {
        uint32_t FFX_FSR2_OPTION_REPROJECT_USE_LANCZOS_TYPE : 1;
        uint32_t FFX_FSR2_OPTION_HDR_COLOR_INPUT : 1;
        uint32_t FFX_FSR2_OPTION_LOW_RESOLUTION_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_JITTERED_MOTION_VECTORS : 1;
        uint32_t FFX_FSR2_OPTION_INVERTED_DEPTH : 1;
        uint32_t FFX_FSR2_OPTION_APPLY_SHARPENING : 1;
    };
    uint32_t index;
} ffx_fsr2_depth_clip_pass_wave64_PermutationKey;

typedef struct ffx_fsr2_depth_clip_pass_wave64_PermutationInfo {
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
} ffx_fsr2_depth_clip_pass_wave64_PermutationInfo;

static const uint32_t g_ffx_fsr2_depth_clip_pass_wave64_IndirectionTable[] = {
    1,
    1,
    1,
    1,
    3,
    3,
    3,
    3,
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
    5,
    5,
    5,
    5,
    4,
    4,
    4,
    4,
    6,
    6,
    6,
    6,
    1,
    1,
    1,
    1,
    3,
    3,
    3,
    3,
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
    5,
    5,
    5,
    5,
    4,
    4,
    4,
    4,
    6,
    6,
    6,
    6,
};

static const ffx_fsr2_depth_clip_pass_wave64_PermutationInfo g_ffx_fsr2_depth_clip_pass_wave64_PermutationInfo[] = {
    { g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_size, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_data, 1, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_ff499387849b4703_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_size, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_data, 1, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_feb48be2e349e5c2_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_size, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_data, 1, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_593dfea81c1600b0_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_size, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_data, 1, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_f70d95daeaacbe52_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_size, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_data, 1, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_4e50713944c33fed_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_size, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_data, 1, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_471869a32d088cee_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_size, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_data, 1, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_f71c046da7bd0d34_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
    { g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_size, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_data, 1, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_CBVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_CBVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_CBVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_CBVResourceSpaces, 9, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_TextureSRVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_TextureSRVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_TextureSRVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_TextureSRVResourceSpaces, 2, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_TextureUAVResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_TextureUAVResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_TextureUAVResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_SamplerResourceNames, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_SamplerResourceBindings, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_SamplerResourceCounts, g_ffx_fsr2_depth_clip_pass_wave64_7198a81b920418b8_SamplerResourceSpaces, 0, 0, 0, 0, 0, },
};

