#include "rcas_c759eaccf971b47f.h"
#include "rcas_cd4432882938866c.h"
#include "rcas_e753886adef53b60.h"
#include "rcas_d2e0a65d52c2d67a.h"
#include "rcas_a3d2c0eb8023e371.h"

typedef union rcas_PermutationKey {
    struct {
        uint32_t FFX_MLSR_COLORSPACE : 2;
        uint32_t FFX_MLSR_AUTOEXPOSURE_ENABLED : 1;
    };
    uint32_t index;
} rcas_PermutationKey;

typedef struct rcas_PermutationInfo {
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
} rcas_PermutationInfo;

static const uint32_t g_rcas_IndirectionTable[] = {
    4,
    0,
    2,
    3,
    1,
    0,
    2,
    3,
};

static const rcas_PermutationInfo g_rcas_PermutationInfo[] = {
    { g_rcas_c759eaccf971b47f_size, g_rcas_c759eaccf971b47f_data, 1, g_rcas_c759eaccf971b47f_CBVResourceNames, g_rcas_c759eaccf971b47f_CBVResourceBindings, g_rcas_c759eaccf971b47f_CBVResourceCounts, g_rcas_c759eaccf971b47f_CBVResourceSpaces, 1, g_rcas_c759eaccf971b47f_TextureSRVResourceNames, g_rcas_c759eaccf971b47f_TextureSRVResourceBindings, g_rcas_c759eaccf971b47f_TextureSRVResourceCounts, g_rcas_c759eaccf971b47f_TextureSRVResourceSpaces, 1, g_rcas_c759eaccf971b47f_TextureUAVResourceNames, g_rcas_c759eaccf971b47f_TextureUAVResourceBindings, g_rcas_c759eaccf971b47f_TextureUAVResourceCounts, g_rcas_c759eaccf971b47f_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_rcas_cd4432882938866c_size, g_rcas_cd4432882938866c_data, 1, g_rcas_cd4432882938866c_CBVResourceNames, g_rcas_cd4432882938866c_CBVResourceBindings, g_rcas_cd4432882938866c_CBVResourceCounts, g_rcas_cd4432882938866c_CBVResourceSpaces, 2, g_rcas_cd4432882938866c_TextureSRVResourceNames, g_rcas_cd4432882938866c_TextureSRVResourceBindings, g_rcas_cd4432882938866c_TextureSRVResourceCounts, g_rcas_cd4432882938866c_TextureSRVResourceSpaces, 1, g_rcas_cd4432882938866c_TextureUAVResourceNames, g_rcas_cd4432882938866c_TextureUAVResourceBindings, g_rcas_cd4432882938866c_TextureUAVResourceCounts, g_rcas_cd4432882938866c_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_rcas_e753886adef53b60_size, g_rcas_e753886adef53b60_data, 1, g_rcas_e753886adef53b60_CBVResourceNames, g_rcas_e753886adef53b60_CBVResourceBindings, g_rcas_e753886adef53b60_CBVResourceCounts, g_rcas_e753886adef53b60_CBVResourceSpaces, 1, g_rcas_e753886adef53b60_TextureSRVResourceNames, g_rcas_e753886adef53b60_TextureSRVResourceBindings, g_rcas_e753886adef53b60_TextureSRVResourceCounts, g_rcas_e753886adef53b60_TextureSRVResourceSpaces, 1, g_rcas_e753886adef53b60_TextureUAVResourceNames, g_rcas_e753886adef53b60_TextureUAVResourceBindings, g_rcas_e753886adef53b60_TextureUAVResourceCounts, g_rcas_e753886adef53b60_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_rcas_d2e0a65d52c2d67a_size, g_rcas_d2e0a65d52c2d67a_data, 1, g_rcas_d2e0a65d52c2d67a_CBVResourceNames, g_rcas_d2e0a65d52c2d67a_CBVResourceBindings, g_rcas_d2e0a65d52c2d67a_CBVResourceCounts, g_rcas_d2e0a65d52c2d67a_CBVResourceSpaces, 1, g_rcas_d2e0a65d52c2d67a_TextureSRVResourceNames, g_rcas_d2e0a65d52c2d67a_TextureSRVResourceBindings, g_rcas_d2e0a65d52c2d67a_TextureSRVResourceCounts, g_rcas_d2e0a65d52c2d67a_TextureSRVResourceSpaces, 1, g_rcas_d2e0a65d52c2d67a_TextureUAVResourceNames, g_rcas_d2e0a65d52c2d67a_TextureUAVResourceBindings, g_rcas_d2e0a65d52c2d67a_TextureUAVResourceCounts, g_rcas_d2e0a65d52c2d67a_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_rcas_a3d2c0eb8023e371_size, g_rcas_a3d2c0eb8023e371_data, 1, g_rcas_a3d2c0eb8023e371_CBVResourceNames, g_rcas_a3d2c0eb8023e371_CBVResourceBindings, g_rcas_a3d2c0eb8023e371_CBVResourceCounts, g_rcas_a3d2c0eb8023e371_CBVResourceSpaces, 2, g_rcas_a3d2c0eb8023e371_TextureSRVResourceNames, g_rcas_a3d2c0eb8023e371_TextureSRVResourceBindings, g_rcas_a3d2c0eb8023e371_TextureSRVResourceCounts, g_rcas_a3d2c0eb8023e371_TextureSRVResourceSpaces, 1, g_rcas_a3d2c0eb8023e371_TextureUAVResourceNames, g_rcas_a3d2c0eb8023e371_TextureUAVResourceBindings, g_rcas_a3d2c0eb8023e371_TextureUAVResourceCounts, g_rcas_a3d2c0eb8023e371_TextureUAVResourceSpaces, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};

