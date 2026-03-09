// This file is part of the FidelityFX SDK.
//
// Copyright (C) 2025 Advanced Micro Devices, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "../../api/internal/ffx_interface.h"
#include "../../api/internal/ffx_util.h"
#include "../../api/internal/ffx_assert.h"

#include "ffx_vk.h"

#include <cstring>
#include <mutex>
#include <limits>
#include <algorithm>

// Maximum number of timestamps supported
#define FFX_MAX_TIMESTAMPS (16)

#define FFX_MAX_RESOURCE_IDENTIFIER_COUNT (128)

// ============================================================================
// VK backend function prototypes
// ============================================================================
FfxVersionNumber GetSDKVersionVK(FfxInterface* backendInterface);
FfxErrorCode GetEffectGpuMemoryUsageVK(FfxInterface* backendInterface, FfxUInt32 effectContextId, FfxApiEffectMemoryUsage* outVramUsage);
FfxErrorCode CreateBackendContextVK(FfxInterface* backendInterface, FfxEffect effect, FfxEffectBindlessConfig* bindlessConfig, FfxUInt32* effectContextId);
FfxErrorCode GetDeviceCapabilitiesVK(FfxInterface* backendInterface, FfxDeviceCapabilities* deviceCapabilities);
FfxErrorCode DestroyBackendContextVK(FfxInterface* backendInterface, FfxUInt32 effectContextId);
FfxErrorCode CreateResourceVK(FfxInterface* backendInterface, const FfxCreateResourceDescription* desc, FfxUInt32 effectContextId, FfxResourceInternal* outTexture);
FfxErrorCode DestroyResourceVK(FfxInterface* backendInterface, FfxResourceInternal resource, FfxUInt32 effectContextId);
FfxErrorCode MapResourceVK(FfxInterface* backendInterface, FfxResourceInternal resource, void** ptr);
FfxErrorCode UnmapResourceVK(FfxInterface* backendInterface, FfxResourceInternal resource);
FfxErrorCode RegisterResourceVK(FfxInterface* backendInterface, const FfxApiResource* inResource, FfxUInt32 effectContextId, FfxResourceInternal* outResourceInternal);
FfxApiResource GetResourceVK(FfxInterface* backendInterface, FfxResourceInternal resource);
FfxErrorCode UnregisterResourcesVK(FfxInterface* backendInterface, FfxCommandList commandList, FfxUInt32 effectContextId);
FfxErrorCode RegisterStaticResourceVK(FfxInterface* backendInterface, const FfxStaticResourceDescription* desc, FfxUInt32 effectContextId);
FfxApiResourceDescription GetResourceDescriptorVK(FfxInterface* backendInterface, FfxResourceInternal resource);
FfxErrorCode StageConstantBufferDataVK(FfxInterface* backendInterface, void* data, FfxUInt32 size, FfxConstantBuffer* constantBuffer);
FfxErrorCode CreatePipelineVK(FfxInterface* backendInterface, FfxShaderBlob* pShaderBlob, const FfxPipelineDescription* desc, FfxUInt32 effectContextId, FfxPipelineState* outPass);
FfxErrorCode DestroyPipelineVK(FfxInterface* backendInterface, FfxPipelineState* pipeline, FfxUInt32 effectContextId);
FfxErrorCode ScheduleGpuJobVK(FfxInterface* backendInterface, const FfxGpuJobDescription* job);
FfxErrorCode ExecuteGpuJobsVK(FfxInterface* backendInterface, FfxCommandList commandList, FfxUInt32 effectContextId);
FfxErrorCode CreateHeapVK(FfxInterface* backendInterface, const FfxCreateHeapDescription* createHeapDescription, FfxUInt32 effectContextId, FfxResourceHeap* outHeap);
FfxErrorCode DestroyHeapVK(FfxInterface* backendInterface, FfxResourceHeap heap, FfxUInt32 effectContextId);

// ============================================================================
// Backend context structure
// ============================================================================
typedef struct BackendContext_VK {

    typedef struct Resource
    {
#ifdef _DEBUG
        wchar_t                     resourceName[64] = {};
#endif
        VkImage                     imageResource;
        VkBuffer                    bufferResource;
        VkDeviceMemory              deviceMemory;
        VkImageView                 imageView;
        VkBufferView                bufferView;
        FfxApiResourceDescription   resourceDescription;
        FfxApiResourceState         initialState;
        FfxApiResourceState         currentState;
        bool                        ownedByBackend;
    } Resource;

    uint32_t refCount;
    uint32_t maxEffectContexts;

    VkDevice            device;
    VkPhysicalDevice    physicalDevice;

    VkPhysicalDeviceProperties          deviceProperties;
    VkPhysicalDeviceMemoryProperties    memoryProperties;

    FfxGpuJobDescription*   pGpuJobs;
    uint32_t                gpuJobCount;

    VkImageMemoryBarrier    barriers[FFX_MAX_BARRIERS];
    uint32_t                barrierCount;

    uint8_t*                pStagingRingBuffer;
    uint32_t                stagingRingBufferBase;

    typedef struct alignas(32) EffectContext {
        uint32_t nextStaticResource;
        uint32_t nextDynamicResource;

        Resource resources[FFX_MAX_RESOURCE_COUNT];
        VkDescriptorPool descriptorPool;
        VkDescriptorSetLayout descriptorSetLayouts[FFX_MAX_PASS_COUNT];
        VkPipelineLayout pipelineLayouts[FFX_MAX_PASS_COUNT];
        VkPipeline computePipelines[FFX_MAX_PASS_COUNT];
        VkShaderModule shaderModules[FFX_MAX_PASS_COUNT];
        VkSampler samplers[FFX_MAX_SAMPLERS];
        uint32_t samplerCount;

        FfxApiEffectMemoryUsage effectMemoryUsage;
    } EffectContext;

    EffectContext* pEffectContexts;

} BackendContext_VK;

// ============================================================================
// Helper: find memory type index
// ============================================================================
static uint32_t FindMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties& memProps,
                                     uint32_t memoryTypeBits,
                                     VkMemoryPropertyFlags requiredFlags)
{
    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i)
    {
        if ((memoryTypeBits & (1u << i)) &&
            (memProps.memoryTypes[i].propertyFlags & requiredFlags) == requiredFlags)
        {
            return i;
        }
    }
    return UINT32_MAX;
}

// ============================================================================
// Helper: VkFormat conversions
// ============================================================================
static VkFormat GetVkFormatFromSurfaceFormat(FfxApiSurfaceFormat surfaceFormat)
{
    return ffxGetVKFormat(surfaceFormat);
}

static FfxApiSurfaceFormat GetSurfaceFormatFromVkFormat(VkFormat format)
{
    return ffxGetSurfaceFormatVK(format);
}

static VkImageUsageFlags GetVkImageUsageFromResourceUsage(FfxApiResourceUsage usage)
{
    VkImageUsageFlags flags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    if (usage & FFX_API_RESOURCE_USAGE_READ_ONLY)
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    if (usage & FFX_API_RESOURCE_USAGE_RENDERTARGET)
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    if (usage & FFX_API_RESOURCE_USAGE_UAV)
        flags |= VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    return flags;
}

static VkImageLayout GetVkImageLayoutFromResourceState(FfxApiResourceState state)
{
    switch (state)
    {
    case FFX_API_RESOURCE_STATE_COMMON:
        return VK_IMAGE_LAYOUT_GENERAL;
    case FFX_API_RESOURCE_STATE_UNORDERED_ACCESS:
        return VK_IMAGE_LAYOUT_GENERAL;
    case FFX_API_RESOURCE_STATE_COMPUTE_READ:
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    case FFX_API_RESOURCE_STATE_PIXEL_READ:
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    case FFX_API_RESOURCE_STATE_PIXEL_COMPUTE_READ:
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    case FFX_API_RESOURCE_STATE_COPY_SRC:
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    case FFX_API_RESOURCE_STATE_COPY_DEST:
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    case FFX_API_RESOURCE_STATE_GENERIC_READ:
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    case FFX_API_RESOURCE_STATE_INDIRECT_ARGUMENT:
        return VK_IMAGE_LAYOUT_GENERAL;
    case FFX_API_RESOURCE_STATE_PRESENT:
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    case FFX_API_RESOURCE_STATE_RENDER_TARGET:
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    default:
        return VK_IMAGE_LAYOUT_GENERAL;
    }
}

static VkAccessFlags GetVkAccessFlagsFromResourceState(FfxApiResourceState state)
{
    switch (state)
    {
    case FFX_API_RESOURCE_STATE_COMMON:
        return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    case FFX_API_RESOURCE_STATE_UNORDERED_ACCESS:
        return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    case FFX_API_RESOURCE_STATE_COMPUTE_READ:
    case FFX_API_RESOURCE_STATE_PIXEL_READ:
    case FFX_API_RESOURCE_STATE_PIXEL_COMPUTE_READ:
    case FFX_API_RESOURCE_STATE_GENERIC_READ:
        return VK_ACCESS_SHADER_READ_BIT;
    case FFX_API_RESOURCE_STATE_COPY_SRC:
        return VK_ACCESS_TRANSFER_READ_BIT;
    case FFX_API_RESOURCE_STATE_COPY_DEST:
        return VK_ACCESS_TRANSFER_WRITE_BIT;
    case FFX_API_RESOURCE_STATE_INDIRECT_ARGUMENT:
        return VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    case FFX_API_RESOURCE_STATE_PRESENT:
        return 0;
    case FFX_API_RESOURCE_STATE_RENDER_TARGET:
        return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    default:
        return 0;
    }
}

// ============================================================================
// Scratch memory sizing
// ============================================================================
size_t ffxGetScratchMemorySizeVK(size_t maxContexts)
{
    size_t backendContextSize = sizeof(BackendContext_VK);
    size_t effectContextsSize = sizeof(BackendContext_VK::EffectContext) * maxContexts;
    size_t gpuJobsSize        = sizeof(FfxGpuJobDescription) * FFX_MAX_GPU_JOBS;
    size_t stagingBufferSize  = FFX_CONSTANT_BUFFER_RING_BUFFER_SIZE * maxContexts;

    return FFX_ALIGN_UP(backendContextSize, 64) +
           FFX_ALIGN_UP(effectContextsSize, 64) +
           FFX_ALIGN_UP(gpuJobsSize, 64) +
           FFX_ALIGN_UP(stagingBufferSize, 64);
}

// ============================================================================
// Device / CommandList / Resource wrappers
// ============================================================================
FfxDevice ffxGetDeviceVK(VkDevice device)
{
    return reinterpret_cast<FfxDevice>(device);
}

FfxCommandList ffxGetCommandListVK(VkCommandBuffer cmdBuffer)
{
    return reinterpret_cast<FfxCommandList>(cmdBuffer);
}

FfxSwapchain ffxGetSwapchainVK(VkSwapchainKHR swapchain)
{
    return reinterpret_cast<FfxSwapchain>(swapchain);
}

VkSwapchainKHR ffxGetVKSwapchain(FfxSwapchain ffxSwapchain)
{
    return reinterpret_cast<VkSwapchainKHR>(ffxSwapchain);
}

FfxApiSurfaceFormat ffxGetSurfaceFormatVK(VkFormat format)
{
    return static_cast<FfxApiSurfaceFormat>(ffxApiGetSurfaceFormatVK(format));
}

VkFormat ffxGetVKFormat(FfxApiSurfaceFormat surfaceFormat)
{
    return ffxGetVKFormatFromSurfaceFormat(surfaceFormat);
}

FfxApiResource ffxGetResourceVK(VkImage image,
                                 FfxApiResourceDescription ffxResDescription,
                                 const wchar_t* ffxResName,
                                 uint32_t state)
{
    (void)ffxResName;
    FfxApiResource res{};
    res.resource = reinterpret_cast<void*>(image);
    res.state = state;
    res.description = ffxResDescription;
    return res;
}

FfxApiResourceDescription ffxGetResourceDescriptionVK(VkImage image, VkFormat format, uint32_t width, uint32_t height, FfxApiResourceUsage additionalUsages)
{
    FfxApiResourceDescription desc{};
    if (!image)
        return desc;

    desc.type = FFX_API_RESOURCE_TYPE_TEXTURE2D;
    desc.format = GetSurfaceFormatFromVkFormat(format);
    desc.width = width;
    desc.height = height;
    desc.depth = 1;
    desc.mipCount = 1;
    desc.flags = FFX_API_RESOURCE_FLAGS_NONE;
    desc.usage = FFX_API_RESOURCE_USAGE_READ_ONLY | additionalUsages;
    return desc;
}

// ============================================================================
// GetSDKVersion
// ============================================================================
FfxVersionNumber GetSDKVersionVK(FfxInterface* backendInterface)
{
    (void)backendInterface;
    return FFX_SDK_MAKE_VERSION(FFX_SDK_VERSION_MAJOR, FFX_SDK_VERSION_MINOR, FFX_SDK_VERSION_PATCH);
}

// ============================================================================
// GetEffectGpuMemoryUsage
// ============================================================================
FfxErrorCode GetEffectGpuMemoryUsageVK(FfxInterface* backendInterface, FfxUInt32 effectContextId, FfxApiEffectMemoryUsage* outVramUsage)
{
    FFX_ASSERT(backendInterface);
    FFX_ASSERT(outVramUsage);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);
    BackendContext_VK::EffectContext& effectCtx = context->pEffectContexts[effectContextId];

    *outVramUsage = effectCtx.effectMemoryUsage;
    return FFX_OK;
}

// ============================================================================
// CreateBackendContext
// ============================================================================
FfxErrorCode CreateBackendContextVK(FfxInterface* backendInterface, FfxEffect effect, FfxEffectBindlessConfig* bindlessConfig, FfxUInt32* effectContextId)
{
    (void)effect;
    (void)bindlessConfig;

    FFX_ASSERT(backendInterface);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);

    // On first context creation, initialize the backend context
    if (context->refCount == 0)
    {
        VkDevice device = reinterpret_cast<VkDevice>(backendInterface->device);
        context->device = device;

        // Physical device must be stored by ffxGetInterfaceVK
        // It's stored right after the BackendContext_VK struct in scratch memory.
        // We set it during ffxGetInterfaceVK.

        vkGetPhysicalDeviceProperties(context->physicalDevice, &context->deviceProperties);
        vkGetPhysicalDeviceMemoryProperties(context->physicalDevice, &context->memoryProperties);
    }

    // Find a free effect context slot
    for (uint32_t i = 0; i < context->maxEffectContexts; ++i)
    {
        if (context->pEffectContexts[i].nextStaticResource == 0 && context->pEffectContexts[i].nextDynamicResource == 0)
        {
            *effectContextId = i;
            BackendContext_VK::EffectContext& effectCtx = context->pEffectContexts[i];
            memset(&effectCtx, 0, sizeof(effectCtx));

            // Reserve slot 0 as null
            effectCtx.nextStaticResource = 1;
            effectCtx.nextDynamicResource = FFX_MAX_RESOURCE_COUNT - 1;

            context->refCount++;
            return FFX_OK;
        }
    }

    return FFX_ERROR_OUT_OF_MEMORY;
}

// ============================================================================
// GetDeviceCapabilities
// ============================================================================
FfxErrorCode GetDeviceCapabilitiesVK(FfxInterface* backendInterface, FfxDeviceCapabilities* deviceCapabilities)
{
    FFX_ASSERT(backendInterface);
    FFX_ASSERT(deviceCapabilities);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);

    // Query subgroup properties for wave lane counts
    VkPhysicalDeviceSubgroupProperties subgroupProps{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES};
    VkPhysicalDeviceProperties2 props2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
    props2.pNext = &subgroupProps;
    vkGetPhysicalDeviceProperties2(context->physicalDevice, &props2);

    deviceCapabilities->maximumSupportedShaderModel = FFX_SHADER_MODEL_6_2;
    deviceCapabilities->waveLaneCountMin = subgroupProps.subgroupSize;
    deviceCapabilities->waveLaneCountMax = subgroupProps.subgroupSize;
    deviceCapabilities->fp16Supported = true;
    deviceCapabilities->raytracingSupported = false;

    // Ensure reasonable fallbacks
    if (deviceCapabilities->waveLaneCountMin == 0)
        deviceCapabilities->waveLaneCountMin = 32;
    if (deviceCapabilities->waveLaneCountMax == 0)
        deviceCapabilities->waveLaneCountMax = 64;

    return FFX_OK;
}

// ============================================================================
// DestroyBackendContext
// ============================================================================
FfxErrorCode DestroyBackendContextVK(FfxInterface* backendInterface, FfxUInt32 effectContextId)
{
    FFX_ASSERT(backendInterface);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);
    BackendContext_VK::EffectContext& effectCtx = context->pEffectContexts[effectContextId];

    // Destroy owned resources
    for (uint32_t i = 0; i < FFX_MAX_RESOURCE_COUNT; ++i)
    {
        BackendContext_VK::Resource& res = effectCtx.resources[i];
        if (res.ownedByBackend)
        {
            if (res.imageView != VK_NULL_HANDLE)
                vkDestroyImageView(context->device, res.imageView, nullptr);
            if (res.bufferView != VK_NULL_HANDLE)
                vkDestroyBufferView(context->device, res.bufferView, nullptr);
            if (res.imageResource != VK_NULL_HANDLE)
                vkDestroyImage(context->device, res.imageResource, nullptr);
            if (res.bufferResource != VK_NULL_HANDLE)
                vkDestroyBuffer(context->device, res.bufferResource, nullptr);
            if (res.deviceMemory != VK_NULL_HANDLE)
                vkFreeMemory(context->device, res.deviceMemory, nullptr);
        }
    }

    // Destroy pipelines
    for (uint32_t i = 0; i < FFX_MAX_PASS_COUNT; ++i)
    {
        if (effectCtx.computePipelines[i] != VK_NULL_HANDLE)
            vkDestroyPipeline(context->device, effectCtx.computePipelines[i], nullptr);
        if (effectCtx.pipelineLayouts[i] != VK_NULL_HANDLE)
            vkDestroyPipelineLayout(context->device, effectCtx.pipelineLayouts[i], nullptr);
        if (effectCtx.descriptorSetLayouts[i] != VK_NULL_HANDLE)
            vkDestroyDescriptorSetLayout(context->device, effectCtx.descriptorSetLayouts[i], nullptr);
        if (effectCtx.shaderModules[i] != VK_NULL_HANDLE)
            vkDestroyShaderModule(context->device, effectCtx.shaderModules[i], nullptr);
    }

    // Destroy samplers
    for (uint32_t i = 0; i < effectCtx.samplerCount; ++i)
    {
        if (effectCtx.samplers[i] != VK_NULL_HANDLE)
            vkDestroySampler(context->device, effectCtx.samplers[i], nullptr);
    }

    // Destroy descriptor pool
    if (effectCtx.descriptorPool != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(context->device, effectCtx.descriptorPool, nullptr);

    memset(&effectCtx, 0, sizeof(effectCtx));

    context->refCount--;

    return FFX_OK;
}

// ============================================================================
// CreateResource
// ============================================================================
FfxErrorCode CreateResourceVK(FfxInterface* backendInterface, const FfxCreateResourceDescription* desc, FfxUInt32 effectContextId, FfxResourceInternal* outResource)
{
    FFX_ASSERT(backendInterface);
    FFX_ASSERT(desc);
    FFX_ASSERT(outResource);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);
    BackendContext_VK::EffectContext& effectCtx = context->pEffectContexts[effectContextId];

    // Allocate a resource slot
    uint32_t resourceIndex = effectCtx.nextStaticResource++;
    if (resourceIndex >= FFX_MAX_RESOURCE_COUNT)
        return FFX_ERROR_OUT_OF_MEMORY;

    BackendContext_VK::Resource& res = effectCtx.resources[resourceIndex];
    memset(&res, 0, sizeof(res));
    res.resourceDescription = desc->resourceDescription;
    res.initialState = desc->initalState;
    res.currentState = desc->initalState;
    res.ownedByBackend = true;

    VkFormat format = GetVkFormatFromSurfaceFormat(static_cast<FfxApiSurfaceFormat>(desc->resourceDescription.format));

    if (desc->resourceDescription.type == FFX_API_RESOURCE_TYPE_BUFFER)
    {
        VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferInfo.size  = desc->resourceDescription.width;
        bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(context->device, &bufferInfo, nullptr, &res.bufferResource);
        if (result != VK_SUCCESS)
            return FFX_ERROR_BACKEND_API_ERROR;

        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(context->device, res.bufferResource, &memReqs);

        VkMemoryAllocateInfo allocInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
        allocInfo.allocationSize = memReqs.size;
        allocInfo.memoryTypeIndex = FindMemoryTypeIndex(context->memoryProperties, memReqs.memoryTypeBits,
                                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        result = vkAllocateMemory(context->device, &allocInfo, nullptr, &res.deviceMemory);
        if (result != VK_SUCCESS)
            return FFX_ERROR_OUT_OF_MEMORY;

        vkBindBufferMemory(context->device, res.bufferResource, res.deviceMemory, 0);
    }
    else
    {
        // Texture (2D or 3D)
        VkImageCreateInfo imageInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        imageInfo.imageType   = (desc->resourceDescription.type == FFX_API_RESOURCE_TYPE_TEXTURE3D) ? VK_IMAGE_TYPE_3D : VK_IMAGE_TYPE_2D;
        imageInfo.format      = format;
        imageInfo.extent      = {desc->resourceDescription.width, desc->resourceDescription.height,
                                 (desc->resourceDescription.depth > 0) ? desc->resourceDescription.depth : 1u};
        imageInfo.mipLevels   = std::max(1u, desc->resourceDescription.mipCount);
        imageInfo.arrayLayers = 1;
        imageInfo.samples     = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling      = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage       = GetVkImageUsageFromResourceUsage(static_cast<FfxApiResourceUsage>(desc->resourceDescription.usage));
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkResult result = vkCreateImage(context->device, &imageInfo, nullptr, &res.imageResource);
        if (result != VK_SUCCESS)
            return FFX_ERROR_BACKEND_API_ERROR;

        VkMemoryRequirements memReqs;
        vkGetImageMemoryRequirements(context->device, res.imageResource, &memReqs);

        VkMemoryAllocateInfo allocInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
        allocInfo.allocationSize = memReqs.size;
        allocInfo.memoryTypeIndex = FindMemoryTypeIndex(context->memoryProperties, memReqs.memoryTypeBits,
                                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        result = vkAllocateMemory(context->device, &allocInfo, nullptr, &res.deviceMemory);
        if (result != VK_SUCCESS)
            return FFX_ERROR_OUT_OF_MEMORY;

        vkBindImageMemory(context->device, res.imageResource, res.deviceMemory, 0);

        // Create default image view
        VkImageViewCreateInfo viewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        viewInfo.image    = res.imageResource;
        viewInfo.viewType = (desc->resourceDescription.type == FFX_API_RESOURCE_TYPE_TEXTURE3D) ? VK_IMAGE_VIEW_TYPE_3D : VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format   = format;
        viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = imageInfo.mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        vkCreateImageView(context->device, &viewInfo, nullptr, &res.imageView);

        // Track memory usage
        effectCtx.effectMemoryUsage.totalUsageInBytes += memReqs.size;
    }

    outResource->internalIndex = resourceIndex;
    return FFX_OK;
}

// ============================================================================
// DestroyResource
// ============================================================================
FfxErrorCode DestroyResourceVK(FfxInterface* backendInterface, FfxResourceInternal resource, FfxUInt32 effectContextId)
{
    FFX_ASSERT(backendInterface);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);
    BackendContext_VK::EffectContext& effectCtx = context->pEffectContexts[effectContextId];

    if (resource.internalIndex == 0)
        return FFX_OK;

    BackendContext_VK::Resource& res = effectCtx.resources[resource.internalIndex];
    if (res.ownedByBackend)
    {
        if (res.imageView != VK_NULL_HANDLE)
            vkDestroyImageView(context->device, res.imageView, nullptr);
        if (res.bufferView != VK_NULL_HANDLE)
            vkDestroyBufferView(context->device, res.bufferView, nullptr);
        if (res.imageResource != VK_NULL_HANDLE)
            vkDestroyImage(context->device, res.imageResource, nullptr);
        if (res.bufferResource != VK_NULL_HANDLE)
            vkDestroyBuffer(context->device, res.bufferResource, nullptr);
        if (res.deviceMemory != VK_NULL_HANDLE)
            vkFreeMemory(context->device, res.deviceMemory, nullptr);
    }

    memset(&res, 0, sizeof(res));
    return FFX_OK;
}

// ============================================================================
// MapResource / UnmapResource
// ============================================================================
FfxErrorCode MapResourceVK(FfxInterface* backendInterface, FfxResourceInternal resource, void** ptr)
{
    // Mapping GPU resources requires a staging buffer approach in Vulkan.
    // For constant buffer data, we use the staging ring buffer.
    (void)backendInterface;
    (void)resource;
    (void)ptr;
    return FFX_ERROR_BACKEND_API_ERROR;
}

FfxErrorCode UnmapResourceVK(FfxInterface* backendInterface, FfxResourceInternal resource)
{
    (void)backendInterface;
    (void)resource;
    return FFX_ERROR_BACKEND_API_ERROR;
}

// ============================================================================
// RegisterResource
// ============================================================================
FfxErrorCode RegisterResourceVK(FfxInterface* backendInterface, const FfxApiResource* inResource, FfxUInt32 effectContextId, FfxResourceInternal* outResourceInternal)
{
    FFX_ASSERT(backendInterface);
    FFX_ASSERT(outResourceInternal);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);
    BackendContext_VK::EffectContext& effectCtx = context->pEffectContexts[effectContextId];

    if (!inResource || !inResource->resource)
    {
        outResourceInternal->internalIndex = 0;
        return FFX_OK;
    }

    // Use a dynamic resource slot (counting down from the top)
    uint32_t resourceIndex = effectCtx.nextDynamicResource--;
    if (resourceIndex == 0)
        return FFX_ERROR_OUT_OF_MEMORY;

    BackendContext_VK::Resource& res = effectCtx.resources[resourceIndex];
    memset(&res, 0, sizeof(res));

    res.imageResource      = reinterpret_cast<VkImage>(inResource->resource);
    res.resourceDescription = inResource->description;
    res.currentState       = static_cast<FfxApiResourceState>(inResource->state);
    res.ownedByBackend     = false;

    outResourceInternal->internalIndex = resourceIndex;
    return FFX_OK;
}

// ============================================================================
// GetResource
// ============================================================================
FfxApiResource GetResourceVK(FfxInterface* backendInterface, FfxResourceInternal resource)
{
    FFX_ASSERT(backendInterface);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);
    FfxApiResource apiResource{};

    if (resource.internalIndex == 0)
        return apiResource;

    // We need to search across effect contexts - use the first active one
    // In practice, the caller knows which context this resource belongs to
    for (uint32_t i = 0; i < context->maxEffectContexts; ++i)
    {
        BackendContext_VK::EffectContext& effectCtx = context->pEffectContexts[i];
        if (effectCtx.nextStaticResource == 0)
            continue;

        BackendContext_VK::Resource& res = effectCtx.resources[resource.internalIndex];
        if (res.imageResource || res.bufferResource)
        {
            apiResource.resource = reinterpret_cast<void*>(res.imageResource ? res.imageResource : (VkImage)res.bufferResource);
            apiResource.description = res.resourceDescription;
            apiResource.state = res.currentState;
            return apiResource;
        }
    }

    return apiResource;
}

// ============================================================================
// UnregisterResources
// ============================================================================
FfxErrorCode UnregisterResourcesVK(FfxInterface* backendInterface, FfxCommandList commandList, FfxUInt32 effectContextId)
{
    (void)commandList;
    FFX_ASSERT(backendInterface);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);
    BackendContext_VK::EffectContext& effectCtx = context->pEffectContexts[effectContextId];

    // Reset dynamic resource counter - these are external resources registered per-frame
    effectCtx.nextDynamicResource = FFX_MAX_RESOURCE_COUNT - 1;

    return FFX_OK;
}

// ============================================================================
// RegisterStaticResource
// ============================================================================
FfxErrorCode RegisterStaticResourceVK(FfxInterface* backendInterface, const FfxStaticResourceDescription* desc, FfxUInt32 effectContextId)
{
    (void)backendInterface;
    (void)desc;
    (void)effectContextId;
    // Static resource registration for bindless - not yet implemented for VK
    return FFX_OK;
}

// ============================================================================
// GetResourceDescription
// ============================================================================
FfxApiResourceDescription GetResourceDescriptorVK(FfxInterface* backendInterface, FfxResourceInternal resource)
{
    FFX_ASSERT(backendInterface);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);

    if (resource.internalIndex == 0)
    {
        FfxApiResourceDescription desc{};
        return desc;
    }

    for (uint32_t i = 0; i < context->maxEffectContexts; ++i)
    {
        BackendContext_VK::EffectContext& effectCtx = context->pEffectContexts[i];
        if (effectCtx.nextStaticResource == 0)
            continue;

        BackendContext_VK::Resource& res = effectCtx.resources[resource.internalIndex];
        if (res.imageResource || res.bufferResource)
            return res.resourceDescription;
    }

    FfxApiResourceDescription desc{};
    return desc;
}

// ============================================================================
// StageConstantBufferData
// ============================================================================
FfxErrorCode StageConstantBufferDataVK(FfxInterface* backendInterface, void* data, FfxUInt32 size, FfxConstantBuffer* constantBuffer)
{
    FFX_ASSERT(backendInterface);
    FFX_ASSERT(data);
    FFX_ASSERT(constantBuffer);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);

    uint32_t alignedSize = FFX_ALIGN_UP(size, 256);

    if (context->pStagingRingBuffer)
    {
        memcpy(context->pStagingRingBuffer + context->stagingRingBufferBase, data, size);
        constantBuffer->data = context->pStagingRingBuffer + context->stagingRingBufferBase;
        constantBuffer->num32BitEntries = size / sizeof(uint32_t);
        context->stagingRingBufferBase += alignedSize;
    }

    return FFX_OK;
}

// ============================================================================
// CreatePipeline
// ============================================================================
FfxErrorCode CreatePipelineVK(FfxInterface* backendInterface, FfxShaderBlob* pShaderBlob, const FfxPipelineDescription* desc, FfxUInt32 effectContextId, FfxPipelineState* outPipeline)
{
    FFX_ASSERT(backendInterface);
    FFX_ASSERT(pShaderBlob);
    FFX_ASSERT(desc);
    FFX_ASSERT(outPipeline);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);
    BackendContext_VK::EffectContext& effectCtx = context->pEffectContexts[effectContextId];

    // Find a free pipeline slot
    uint32_t pipelineIndex = UINT32_MAX;
    for (uint32_t i = 0; i < FFX_MAX_PASS_COUNT; ++i)
    {
        if (effectCtx.computePipelines[i] == VK_NULL_HANDLE)
        {
            pipelineIndex = i;
            break;
        }
    }

    if (pipelineIndex == UINT32_MAX)
        return FFX_ERROR_OUT_OF_MEMORY;

    // Create shader module from SPIR-V blob
    VkShaderModuleCreateInfo moduleInfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    moduleInfo.codeSize = pShaderBlob->size;
    moduleInfo.pCode    = reinterpret_cast<const uint32_t*>(pShaderBlob->data);

    VkResult result = vkCreateShaderModule(context->device, &moduleInfo, nullptr, &effectCtx.shaderModules[pipelineIndex]);
    if (result != VK_SUCCESS)
        return FFX_ERROR_BACKEND_API_ERROR;

    // Create descriptor set layout from shader blob binding info
    VkDescriptorSetLayoutBinding bindings[FFX_MAX_NUM_SRVS + FFX_MAX_NUM_UAVS + FFX_MAX_NUM_CONST_BUFFERS + FFX_MAX_SAMPLERS];
    uint32_t bindingCount = 0;

    // SRV Textures
    for (uint32_t i = 0; i < pShaderBlob->srvTextureCount; ++i)
    {
        auto& b = bindings[bindingCount++];
        b = {};
        b.binding         = pShaderBlob->boundSRVTextures[i];
        b.descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        b.descriptorCount = pShaderBlob->boundSRVTextureCounts ? pShaderBlob->boundSRVTextureCounts[i] : 1;
        b.stageFlags      = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    // UAV Textures
    for (uint32_t i = 0; i < pShaderBlob->uavTextureCount; ++i)
    {
        auto& b = bindings[bindingCount++];
        b = {};
        b.binding         = pShaderBlob->boundUAVTextures[i];
        b.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        b.descriptorCount = pShaderBlob->boundUAVTextureCounts ? pShaderBlob->boundUAVTextureCounts[i] : 1;
        b.stageFlags      = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    // SRV Buffers
    for (uint32_t i = 0; i < pShaderBlob->srvBufferCount; ++i)
    {
        auto& b = bindings[bindingCount++];
        b = {};
        b.binding         = pShaderBlob->boundSRVBuffers[i];
        b.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        b.descriptorCount = pShaderBlob->boundSRVBufferCounts ? pShaderBlob->boundSRVBufferCounts[i] : 1;
        b.stageFlags      = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    // UAV Buffers
    for (uint32_t i = 0; i < pShaderBlob->uavBufferCount; ++i)
    {
        auto& b = bindings[bindingCount++];
        b = {};
        b.binding         = pShaderBlob->boundUAVBuffers[i];
        b.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        b.descriptorCount = pShaderBlob->boundUAVBufferCounts ? pShaderBlob->boundUAVBufferCounts[i] : 1;
        b.stageFlags      = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    // Constant buffers
    for (uint32_t i = 0; i < pShaderBlob->cbvCount; ++i)
    {
        auto& b = bindings[bindingCount++];
        b = {};
        b.binding         = pShaderBlob->boundConstantBuffers[i];
        b.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        b.descriptorCount = pShaderBlob->boundConstantBufferCounts ? pShaderBlob->boundConstantBufferCounts[i] : 1;
        b.stageFlags      = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    // Samplers
    for (uint32_t i = 0; i < pShaderBlob->samplerCount; ++i)
    {
        auto& b = bindings[bindingCount++];
        b = {};
        b.binding         = pShaderBlob->boundSamplers[i];
        b.descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLER;
        b.descriptorCount = pShaderBlob->boundSamplerCounts ? pShaderBlob->boundSamplerCounts[i] : 1;
        b.stageFlags      = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    layoutInfo.bindingCount = bindingCount;
    layoutInfo.pBindings    = bindings;

    result = vkCreateDescriptorSetLayout(context->device, &layoutInfo, nullptr, &effectCtx.descriptorSetLayouts[pipelineIndex]);
    if (result != VK_SUCCESS)
        return FFX_ERROR_BACKEND_API_ERROR;

    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts    = &effectCtx.descriptorSetLayouts[pipelineIndex];

    // Push constants for constant buffer data
    VkPushConstantRange pushRange{};
    if (pShaderBlob->cbvCount > 0)
    {
        pushRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        pushRange.offset     = 0;
        pushRange.size       = FFX_BUFFER_SIZE;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges    = &pushRange;
    }

    result = vkCreatePipelineLayout(context->device, &pipelineLayoutInfo, nullptr, &effectCtx.pipelineLayouts[pipelineIndex]);
    if (result != VK_SUCCESS)
        return FFX_ERROR_BACKEND_API_ERROR;

    // Create compute pipeline
    VkComputePipelineCreateInfo pipelineInfo{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
    pipelineInfo.stage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineInfo.stage.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineInfo.stage.module = effectCtx.shaderModules[pipelineIndex];
    pipelineInfo.stage.pName  = "CS";
    pipelineInfo.layout       = effectCtx.pipelineLayouts[pipelineIndex];

    result = vkCreateComputePipelines(context->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &effectCtx.computePipelines[pipelineIndex]);
    if (result != VK_SUCCESS)
        return FFX_ERROR_BACKEND_API_ERROR;

    // Populate outPipeline
    memset(outPipeline, 0, sizeof(*outPipeline));
    outPipeline->pipeline      = reinterpret_cast<FfxPipeline>(effectCtx.computePipelines[pipelineIndex]);
    outPipeline->rootSignature = reinterpret_cast<FfxRootSignature>(effectCtx.pipelineLayouts[pipelineIndex]);

    // Copy binding info from shader blob into pipeline state
    outPipeline->srvTextureCount = pShaderBlob->srvTextureCount;
    outPipeline->uavTextureCount = pShaderBlob->uavTextureCount;
    outPipeline->srvBufferCount  = pShaderBlob->srvBufferCount;
    outPipeline->uavBufferCount  = pShaderBlob->uavBufferCount;
    outPipeline->constCount      = pShaderBlob->cbvCount;

    for (uint32_t i = 0; i < pShaderBlob->srvTextureCount && i < FFX_MAX_NUM_SRVS; ++i)
    {
        outPipeline->srvTextureBindings[i].slotIndex    = pShaderBlob->boundSRVTextures[i];
        outPipeline->srvTextureBindings[i].resourceIdentifier = i;
    }
    for (uint32_t i = 0; i < pShaderBlob->uavTextureCount && i < FFX_MAX_NUM_UAVS; ++i)
    {
        outPipeline->uavTextureBindings[i].slotIndex    = pShaderBlob->boundUAVTextures[i];
        outPipeline->uavTextureBindings[i].resourceIdentifier = i;
    }
    for (uint32_t i = 0; i < pShaderBlob->srvBufferCount && i < FFX_MAX_NUM_SRVS; ++i)
    {
        outPipeline->srvBufferBindings[i].slotIndex    = pShaderBlob->boundSRVBuffers[i];
        outPipeline->srvBufferBindings[i].resourceIdentifier = i;
    }
    for (uint32_t i = 0; i < pShaderBlob->uavBufferCount && i < FFX_MAX_NUM_UAVS; ++i)
    {
        outPipeline->uavBufferBindings[i].slotIndex    = pShaderBlob->boundUAVBuffers[i];
        outPipeline->uavBufferBindings[i].resourceIdentifier = i;
    }
    for (uint32_t i = 0; i < pShaderBlob->cbvCount && i < FFX_MAX_NUM_CONST_BUFFERS; ++i)
    {
        outPipeline->constantBufferBindings[i].slotIndex    = pShaderBlob->boundConstantBuffers[i];
        outPipeline->constantBufferBindings[i].resourceIdentifier = i;
    }

    if (desc->name[0])
        wcsncpy(outPipeline->name, desc->name, FFX_RESOURCE_NAME_SIZE - 1);

    return FFX_OK;
}

// ============================================================================
// DestroyPipeline
// ============================================================================
FfxErrorCode DestroyPipelineVK(FfxInterface* backendInterface, FfxPipelineState* pipeline, FfxUInt32 effectContextId)
{
    FFX_ASSERT(backendInterface);
    FFX_ASSERT(pipeline);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);
    BackendContext_VK::EffectContext& effectCtx = context->pEffectContexts[effectContextId];

    VkPipeline vkPipeline = reinterpret_cast<VkPipeline>(pipeline->pipeline);

    for (uint32_t i = 0; i < FFX_MAX_PASS_COUNT; ++i)
    {
        if (effectCtx.computePipelines[i] == vkPipeline)
        {
            vkDestroyPipeline(context->device, effectCtx.computePipelines[i], nullptr);
            effectCtx.computePipelines[i] = VK_NULL_HANDLE;

            if (effectCtx.pipelineLayouts[i] != VK_NULL_HANDLE)
            {
                vkDestroyPipelineLayout(context->device, effectCtx.pipelineLayouts[i], nullptr);
                effectCtx.pipelineLayouts[i] = VK_NULL_HANDLE;
            }
            if (effectCtx.descriptorSetLayouts[i] != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorSetLayout(context->device, effectCtx.descriptorSetLayouts[i], nullptr);
                effectCtx.descriptorSetLayouts[i] = VK_NULL_HANDLE;
            }
            if (effectCtx.shaderModules[i] != VK_NULL_HANDLE)
            {
                vkDestroyShaderModule(context->device, effectCtx.shaderModules[i], nullptr);
                effectCtx.shaderModules[i] = VK_NULL_HANDLE;
            }
            break;
        }
    }

    memset(pipeline, 0, sizeof(*pipeline));
    return FFX_OK;
}

// ============================================================================
// ScheduleGpuJob
// ============================================================================
FfxErrorCode ScheduleGpuJobVK(FfxInterface* backendInterface, const FfxGpuJobDescription* job)
{
    FFX_ASSERT(backendInterface);
    FFX_ASSERT(job);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);

    if (context->gpuJobCount >= FFX_MAX_GPU_JOBS)
        return FFX_ERROR_OUT_OF_MEMORY;

    context->pGpuJobs[context->gpuJobCount++] = *job;
    return FFX_OK;
}

// ============================================================================
// ExecuteGpuJobs
// ============================================================================
static void ExecuteClearJobVK(BackendContext_VK* context, FfxGpuJobDescription* job, VkCommandBuffer cmdBuffer, uint32_t effectContextId)
{
    (void)context;

    VkImage image = reinterpret_cast<VkImage>(job->clearJobDescriptor.target.resource);
    if (!image)
        return;

    VkClearColorValue clearColor{};
    memcpy(clearColor.float32, job->clearJobDescriptor.color, sizeof(float) * 4);

    VkImageSubresourceRange range{};
    range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel   = 0;
    range.levelCount     = 1;
    range.baseArrayLayer = 0;
    range.layerCount     = 1;

    // Transition to TRANSFER_DST
    VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.oldLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.image         = image;
    barrier.subresourceRange = range;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0, 0, nullptr, 0, nullptr, 1, &barrier);

    vkCmdClearColorImage(cmdBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &range);

    // Transition to GENERAL
    barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout     = VK_IMAGE_LAYOUT_GENERAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                         0, 0, nullptr, 0, nullptr, 1, &barrier);
}

static void ExecuteCopyJobVK(BackendContext_VK* context, FfxGpuJobDescription* job, VkCommandBuffer cmdBuffer, uint32_t effectContextId)
{
    (void)context;
    (void)effectContextId;

    VkImage srcImage = reinterpret_cast<VkImage>(job->copyJobDescriptor.src.resource);
    VkImage dstImage = reinterpret_cast<VkImage>(job->copyJobDescriptor.dst.resource);

    if (!srcImage || !dstImage)
        return;

    // Simple image copy
    VkImageCopy region{};
    region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.srcSubresource.layerCount = 1;
    region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.dstSubresource.layerCount = 1;
    region.extent = {job->copyJobDescriptor.src.description.width,
                     job->copyJobDescriptor.src.description.height, 1};

    vkCmdCopyImage(cmdBuffer,
                   srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1, &region);
}

static void ExecuteComputeJobVK(BackendContext_VK* context, FfxGpuJobDescription* job, VkCommandBuffer cmdBuffer, uint32_t effectContextId)
{
    (void)context;
    (void)effectContextId;

    VkPipeline pipeline = reinterpret_cast<VkPipeline>(job->computeJobDescriptor.pipeline.pipeline);
    VkPipelineLayout layout = reinterpret_cast<VkPipelineLayout>(job->computeJobDescriptor.pipeline.rootSignature);

    if (!pipeline || !layout)
        return;

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

    // Push constants for CBs
    for (uint32_t i = 0; i < job->computeJobDescriptor.pipeline.constCount; ++i)
    {
        FfxConstantBuffer& cb = job->computeJobDescriptor.cbs[i];
        if (cb.data && cb.num32BitEntries > 0)
        {
            uint32_t size = cb.num32BitEntries * sizeof(uint32_t);
            vkCmdPushConstants(cmdBuffer, layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, size, cb.data);
        }
    }

    // Dispatch
    vkCmdDispatch(cmdBuffer,
                  job->computeJobDescriptor.dimensions[0],
                  job->computeJobDescriptor.dimensions[1],
                  job->computeJobDescriptor.dimensions[2]);
}

FfxErrorCode ExecuteGpuJobsVK(FfxInterface* backendInterface, FfxCommandList commandList, FfxUInt32 effectContextId)
{
    FFX_ASSERT(backendInterface);

    BackendContext_VK* context = reinterpret_cast<BackendContext_VK*>(backendInterface->scratchBuffer);
    VkCommandBuffer cmdBuffer = reinterpret_cast<VkCommandBuffer>(commandList);

    for (uint32_t i = 0; i < context->gpuJobCount; ++i)
    {
        FfxGpuJobDescription* job = &context->pGpuJobs[i];

        switch (job->jobType)
        {
        case FFX_GPU_JOB_CLEAR_FLOAT:
            ExecuteClearJobVK(context, job, cmdBuffer, effectContextId);
            break;
        case FFX_GPU_JOB_COPY:
            ExecuteCopyJobVK(context, job, cmdBuffer, effectContextId);
            break;
        case FFX_GPU_JOB_COMPUTE:
            ExecuteComputeJobVK(context, job, cmdBuffer, effectContextId);
            break;
        default:
            break;
        }
    }

    context->gpuJobCount = 0;
    return FFX_OK;
}

// ============================================================================
// CreateHeap / DestroyHeap
// ============================================================================
FfxErrorCode CreateHeapVK(FfxInterface* backendInterface, const FfxCreateHeapDescription* createHeapDescription, FfxUInt32 effectContextId, FfxResourceHeap* outHeap)
{
    (void)backendInterface;
    (void)createHeapDescription;
    (void)effectContextId;
    (void)outHeap;
    // Heap-based placement not yet implemented for VK backend
    return FFX_OK;
}

FfxErrorCode DestroyHeapVK(FfxInterface* backendInterface, FfxResourceHeap heap, FfxUInt32 effectContextId)
{
    (void)backendInterface;
    (void)heap;
    (void)effectContextId;
    return FFX_OK;
}

// ============================================================================
// ffxGetInterfaceVK - populate the FfxInterface function pointer table
// ============================================================================
FfxErrorCode ffxGetInterfaceVK(
    FfxInterface* backendInterface,
    FfxDevice device,
    VkPhysicalDevice physicalDevice,
    void* scratchBuffer,
    size_t scratchBufferSize,
    size_t maxContexts)
{
    FFX_RETURN_ON_ERROR(backendInterface, FFX_ERROR_INVALID_POINTER);
    FFX_RETURN_ON_ERROR(scratchBuffer, FFX_ERROR_INVALID_POINTER);
    FFX_RETURN_ON_ERROR(scratchBufferSize >= ffxGetScratchMemorySizeVK(maxContexts), FFX_ERROR_INSUFFICIENT_MEMORY);

    backendInterface->fpGetSDKVersion             = GetSDKVersionVK;
    backendInterface->fpGetEffectGpuMemoryUsage    = GetEffectGpuMemoryUsageVK;
    backendInterface->fpCreateBackendContext       = CreateBackendContextVK;
    backendInterface->fpGetDeviceCapabilities      = GetDeviceCapabilitiesVK;
    backendInterface->fpDestroyBackendContext       = DestroyBackendContextVK;
    backendInterface->fpCreateResource             = CreateResourceVK;
    backendInterface->fpDestroyResource            = DestroyResourceVK;
    backendInterface->fpMapResource                = MapResourceVK;
    backendInterface->fpUnmapResource              = UnmapResourceVK;
    backendInterface->fpGetResource                = GetResourceVK;
    backendInterface->fpRegisterResource           = RegisterResourceVK;
    backendInterface->fpUnregisterResources        = UnregisterResourcesVK;
    backendInterface->fpRegisterStaticResource     = RegisterStaticResourceVK;
    backendInterface->fpGetResourceDescription     = GetResourceDescriptorVK;
    backendInterface->fpStageConstantBufferDataFunc = StageConstantBufferDataVK;
    backendInterface->fpCreatePipeline             = CreatePipelineVK;
    backendInterface->fpDestroyPipeline            = DestroyPipelineVK;
    backendInterface->fpScheduleGpuJob             = ScheduleGpuJobVK;
    backendInterface->fpExecuteGpuJobs             = ExecuteGpuJobsVK;

#if defined(FFX_FRAMEGENERATION)
    backendInterface->fpSwapChainConfigureFrameGeneration = ffxSetFrameGenerationConfigToSwapchainVK;
    backendInterface->fpGetSwapchainABI = nullptr; // VK doesn't use the ABI version mechanism
#else
    backendInterface->fpSwapChainConfigureFrameGeneration = nullptr;
    backendInterface->fpGetSwapchainABI = nullptr;
#endif

    backendInterface->fpCreateHeap  = CreateHeapVK;
    backendInterface->fpDestroyHeap = DestroyHeapVK;

    // Memory assignments
    backendInterface->scratchBuffer     = scratchBuffer;
    backendInterface->scratchBufferSize = scratchBufferSize;

    BackendContext_VK* backendContext = reinterpret_cast<BackendContext_VK*>(scratchBuffer);

    FFX_RETURN_ON_ERROR(!backendContext->refCount, FFX_ERROR_BACKEND_API_ERROR);

    // Clear everything out
    memset(backendContext, 0, sizeof(*backendContext));

    // Store device handles
    backendInterface->device = device;
    backendContext->device = reinterpret_cast<VkDevice>(device);
    backendContext->physicalDevice = physicalDevice;

    // Assign max contexts
    backendContext->maxEffectContexts = static_cast<uint32_t>(maxContexts);

    // Set up sub-allocations from scratch buffer
    uint8_t* scratchPtr = reinterpret_cast<uint8_t*>(scratchBuffer);
    size_t offset = FFX_ALIGN_UP(sizeof(BackendContext_VK), 64);

    backendContext->pEffectContexts = reinterpret_cast<BackendContext_VK::EffectContext*>(scratchPtr + offset);
    offset += FFX_ALIGN_UP(sizeof(BackendContext_VK::EffectContext) * maxContexts, 64);

    backendContext->pGpuJobs = reinterpret_cast<FfxGpuJobDescription*>(scratchPtr + offset);
    offset += FFX_ALIGN_UP(sizeof(FfxGpuJobDescription) * FFX_MAX_GPU_JOBS, 64);

    backendContext->pStagingRingBuffer = scratchPtr + offset;
    backendContext->stagingRingBufferBase = 0;

    return FFX_OK;
}

// ============================================================================
// Frame interpolation swapchain free-function implementations
// (These call into FrameInterpolationSwapchainVK via the global registry)
// ============================================================================

// Forward-declared - implemented in FrameInterpolationSwapchainVK.cpp
// These are already defined there, so we don't need to duplicate them here.
