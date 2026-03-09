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

#include "FrameInterpolationSwapchainVK.h"
#include <unordered_map>
#include <cstring>
#include <algorithm>

// ---------------------------------------------------------------------------
// Global proxy registry
// ---------------------------------------------------------------------------
static std::mutex                                               g_registryMutex;
static std::unordered_map<VkSwapchainKHR, FrameInterpolationSwapchainVK*> g_registry;

FrameInterpolationSwapchainVK* ffxGetFrameInterpolationSwapchainVK(VkSwapchainKHR swapchain)
{
    std::lock_guard<std::mutex> lock(g_registryMutex);
    auto it = g_registry.find(swapchain);
    return (it != g_registry.end()) ? it->second : nullptr;
}

void ffxRegisterFrameInterpolationSwapchainVK(VkSwapchainKHR handle, FrameInterpolationSwapchainVK* proxy)
{
    std::lock_guard<std::mutex> lock(g_registryMutex);
    g_registry[handle] = proxy;
}

void ffxUnregisterFrameInterpolationSwapchainVK(VkSwapchainKHR handle)
{
    std::lock_guard<std::mutex> lock(g_registryMutex);
    g_registry.erase(handle);
}

// ---------------------------------------------------------------------------
// ReplacementResourceVK
// ---------------------------------------------------------------------------
void ReplacementResourceVK::destroy(VkDevice device)
{
    if (view != VK_NULL_HANDLE)   { vkDestroyImageView(device, view, nullptr); view = VK_NULL_HANDLE; }
    if (image != VK_NULL_HANDLE)  { vkDestroyImage(device, image, nullptr); image = VK_NULL_HANDLE; }
    if (memory != VK_NULL_HANDLE) { vkFreeMemory(device, memory, nullptr); memory = VK_NULL_HANDLE; }
    width = height = 0;
    format = VK_FORMAT_UNDEFINED;
    availabilitySemaphoreValue = 0;
}

static uint32_t findMemoryType(VkPhysicalDevice physDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }
    return UINT32_MAX;
}

bool ReplacementResourceVK::create(VkDevice device, VkPhysicalDevice physDevice, uint32_t w, uint32_t h, VkFormat fmt, VkImageUsageFlags usage, uint32_t queueFamilyIndex)
{
    width = w;
    height = h;
    format = fmt;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.format        = fmt;
    imageInfo.extent        = {w, h, 1};
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = 1;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage         = usage;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.queueFamilyIndexCount = 1;
    imageInfo.pQueueFamilyIndices   = &queueFamilyIndex;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
        return false;

    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device, image, &memReqs);

    uint32_t memTypeIdx = findMemoryType(physDevice, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (memTypeIdx == UINT32_MAX)
    {
        vkDestroyImage(device, image, nullptr);
        image = VK_NULL_HANDLE;
        return false;
    }

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = memTypeIdx;

    if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
    {
        vkDestroyImage(device, image, nullptr);
        image = VK_NULL_HANDLE;
        return false;
    }

    vkBindImageMemory(device, image, memory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image    = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format   = ffxConvertFormatSrvVK(fmt);
    viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    if (vkCreateImageView(device, &viewInfo, nullptr, &view) != VK_SUCCESS)
    {
        vkFreeMemory(device, memory, nullptr);
        vkDestroyImage(device, image, nullptr);
        memory = VK_NULL_HANDLE;
        image = VK_NULL_HANDLE;
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// FrameInterpolationSwapchainVK
// ---------------------------------------------------------------------------

FrameInterpolationSwapchainVK::FrameInterpolationSwapchainVK()
{
    memset(&currentConfig, 0, sizeof(currentConfig));
    currentConfig.header.type = FFX_API_FRAME_GENERATION_CONFIG;
}

FrameInterpolationSwapchainVK::~FrameInterpolationSwapchainVK()
{
    shutdown();
}

void FrameInterpolationSwapchainVK::createSyncPrimitives()
{
    VkSemaphoreTypeCreateInfo timelineInfo{};
    timelineInfo.sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineInfo.initialValue  = 0;

    VkSemaphoreCreateInfo semInfo{};
    semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semInfo.pNext = &timelineInfo;

    vkCreateSemaphore(device, &semInfo, nullptr, &gameSemaphore);
    vkCreateSemaphore(device, &semInfo, nullptr, &interpolationSemaphore);
    vkCreateSemaphore(device, &semInfo, nullptr, &presentSemaphore);
    vkCreateSemaphore(device, &semInfo, nullptr, &replacementBufferSemaphore);

    // Binary semaphores for present synchronization
    semInfo.pNext = nullptr;
    vkCreateSemaphore(device, &semInfo, nullptr, &imageAvailableSemaphore);
    vkCreateSemaphore(device, &semInfo, nullptr, &renderFinishedSemaphore);
}

void FrameInterpolationSwapchainVK::destroySyncPrimitives()
{
    auto destroySem = [this](VkSemaphore& sem) {
        if (sem != VK_NULL_HANDLE) { vkDestroySemaphore(device, sem, nullptr); sem = VK_NULL_HANDLE; }
    };
    destroySem(gameSemaphore);
    destroySem(interpolationSemaphore);
    destroySem(presentSemaphore);
    destroySem(replacementBufferSemaphore);
    destroySem(imageAvailableSemaphore);
    destroySem(renderFinishedSemaphore);
}

bool FrameInterpolationSwapchainVK::findPresentQueue()
{
    // Try to find a second graphics queue that supports present for the surface.
    // Fall back to the game queue if not found.
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    // First try: find a different queue family with graphics + present support
    for (uint32_t i = 0; i < queueFamilyCount; ++i)
    {
        if (i == gameQueueFamilyIndex) continue; // skip the game's queue family
        if (!(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) continue;

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        if (presentSupport)
        {
            presentQueueFamilyIndex = i;
            vkGetDeviceQueue(device, i, 0, &presentQueue);
            return true;
        }
    }

    // Second try: find an additional queue in the same family
    if (queueFamilies[gameQueueFamilyIndex].queueCount > 1)
    {
        presentQueueFamilyIndex = gameQueueFamilyIndex;
        vkGetDeviceQueue(device, gameQueueFamilyIndex, 1, &presentQueue);
        return true;
    }

    // Fall back to the game queue itself
    presentQueueFamilyIndex = gameQueueFamilyIndex;
    presentQueue = gameQueue;
    return true;
}

bool FrameInterpolationSwapchainVK::createReplacementResources()
{
    VkImageUsageFlags backBufferUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                        VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                        VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                        VK_IMAGE_USAGE_SAMPLED_BIT |
                                        VK_IMAGE_USAGE_STORAGE_BIT;

    totalUsageInBytes = 0;

    for (uint32_t i = 0; i < swapchainImageCount; ++i)
    {
        if (!replacementBackBuffers[i].create(device, physicalDevice,
                                              swapchainWidth, swapchainHeight,
                                              swapchainFormat, backBufferUsage,
                                              gameQueueFamilyIndex))
            return false;

        totalUsageInBytes += getResourceGpuMemorySizeVK(device, replacementBackBuffers[i].image);
    }

    VkImageUsageFlags interpUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                    VK_IMAGE_USAGE_SAMPLED_BIT |
                                    VK_IMAGE_USAGE_STORAGE_BIT;

    for (int i = 0; i < FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_INTERPOLATION_OUTPUTS_COUNT; ++i)
    {
        if (!interpolationOutputs[i].create(device, physicalDevice,
                                            swapchainWidth, swapchainHeight,
                                            swapchainFormat, interpUsage,
                                            gameQueueFamilyIndex))
            return false;

        totalUsageInBytes += getResourceGpuMemorySizeVK(device, interpolationOutputs[i].image);
    }

    return true;
}

void FrameInterpolationSwapchainVK::destroyReplacementResources()
{
    for (uint32_t i = 0; i < FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_MAX_IMAGE_COUNT; ++i)
        replacementBackBuffers[i].destroy(device);
    for (int i = 0; i < FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_INTERPOLATION_OUTPUTS_COUNT; ++i)
        interpolationOutputs[i].destroy(device);
    uiReplacementBuffer.destroy(device);

    totalUsageInBytes = 0;
}

FfxErrorCode FrameInterpolationSwapchainVK::init(VkDevice dev, VkPhysicalDevice physDev,
                                                  VkQueue gQueue, uint32_t gQueueFamilyIndex,
                                                  VkSwapchainKHR realSC, VkSurfaceKHR surf)
{
    device = dev;
    physicalDevice = physDev;
    gameQueue = gQueue;
    gameQueueFamilyIndex = gQueueFamilyIndex;
    realSwapchain = realSC;
    surface = surf;

    // Query swapchain images
    vkGetSwapchainImagesKHR(device, realSwapchain, &swapchainImageCount, nullptr);
    if (swapchainImageCount > FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_MAX_IMAGE_COUNT)
        swapchainImageCount = FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_MAX_IMAGE_COUNT;
    vkGetSwapchainImagesKHR(device, realSwapchain, &swapchainImageCount, swapchainImages);

    // Query swapchain format & extent from surface capabilities + image
    // We need the actual create info, but since we're wrapping we'll query the surface
    VkSurfaceCapabilitiesKHR surfCaps{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps);
    swapchainWidth = surfCaps.currentExtent.width;
    swapchainHeight = surfCaps.currentExtent.height;

    // Query supported formats to determine the actual swapchain format
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());
    swapchainFormat = surfaceFormats.empty() ? VK_FORMAT_B8G8R8A8_UNORM : surfaceFormats[0].format;

    findPresentQueue();
    createSyncPrimitives();

    presentCommandPool.init(device, presentQueueFamilyIndex, FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_COMMAND_POOL_SIZE);
    interpolationCommandPool.init(device, gameQueueFamilyIndex, FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_COMMAND_POOL_SIZE);

    if (!createReplacementResources())
        return FFX_ERROR_BACKEND_API_ERROR;

    return FFX_OK;
}

FfxErrorCode FrameInterpolationSwapchainVK::initNew(VkDevice dev, VkPhysicalDevice physDev,
                                                     VkQueue gQueue, uint32_t gQueueFamilyIndex,
                                                     const VkSwapchainCreateInfoKHR* createInfo)
{
    device = dev;
    physicalDevice = physDev;
    gameQueue = gQueue;
    gameQueueFamilyIndex = gQueueFamilyIndex;
    surface = createInfo->surface;
    swapchainFormat = createInfo->imageFormat;
    swapchainWidth = createInfo->imageExtent.width;
    swapchainHeight = createInfo->imageExtent.height;

    VkResult result = vkCreateSwapchainKHR(device, createInfo, nullptr, &realSwapchain);
    if (result != VK_SUCCESS)
        return FFX_ERROR_BACKEND_API_ERROR;

    vkGetSwapchainImagesKHR(device, realSwapchain, &swapchainImageCount, nullptr);
    if (swapchainImageCount > FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_MAX_IMAGE_COUNT)
        swapchainImageCount = FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_MAX_IMAGE_COUNT;
    vkGetSwapchainImagesKHR(device, realSwapchain, &swapchainImageCount, swapchainImages);

    findPresentQueue();
    createSyncPrimitives();

    presentCommandPool.init(device, presentQueueFamilyIndex, FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_COMMAND_POOL_SIZE);
    interpolationCommandPool.init(device, gameQueueFamilyIndex, FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_COMMAND_POOL_SIZE);

    if (!createReplacementResources())
        return FFX_ERROR_BACKEND_API_ERROR;

    return FFX_OK;
}

void FrameInterpolationSwapchainVK::shutdown()
{
    killPresenterThread();

    if (device != VK_NULL_HANDLE)
        vkDeviceWaitIdle(device);

    presentCommandPool.destroy();
    interpolationCommandPool.destroy();
    destroyReplacementResources();
    destroySyncPrimitives();

    if (realSwapchain != VK_NULL_HANDLE)
    {
        ffxUnregisterFrameInterpolationSwapchainVK(realSwapchain);
        // Don't destroy the real swapchain - the caller owns it
    }

    device = VK_NULL_HANDLE;
}

void FrameInterpolationSwapchainVK::transitionImageLayout(VkCommandBuffer cmdBuf, VkImage image,
                                                           VkImageLayout oldLayout, VkImageLayout newLayout,
                                                           VkAccessFlags srcAccess, VkAccessFlags dstAccess,
                                                           VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage,
                                                           uint32_t srcQueueFamily, uint32_t dstQueueFamily)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = oldLayout;
    barrier.newLayout                       = newLayout;
    barrier.srcAccessMask                   = srcAccess;
    barrier.dstAccessMask                   = dstAccess;
    barrier.srcQueueFamilyIndex             = srcQueueFamily;
    barrier.dstQueueFamilyIndex             = dstQueueFamily;
    barrier.image                           = image;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;

    vkCmdPipelineBarrier(cmdBuf, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void FrameInterpolationSwapchainVK::blitImage(VkCommandBuffer cmdBuf, VkImage src, VkImage dst,
                                               VkImageLayout srcLayout, VkImageLayout dstLayout,
                                               uint32_t width, uint32_t height)
{
    // Transition src to TRANSFER_SRC, dst to TRANSFER_DST
    transitionImageLayout(cmdBuf, src, srcLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                          VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                          VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
    transitionImageLayout(cmdBuf, dst, dstLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                          VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

    VkImageCopy region{};
    region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.srcSubresource.layerCount = 1;
    region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.dstSubresource.layerCount = 1;
    region.extent = {width, height, 1};

    vkCmdCopyImage(cmdBuf, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

VkImage FrameInterpolationSwapchainVK::getBackBufferImage(uint32_t index) const
{
    if (index < swapchainImageCount)
        return replacementBackBuffers[index].image;
    return VK_NULL_HANDLE;
}

VkResult FrameInterpolationSwapchainVK::acquireNextImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex)
{
    // Return a replacement back buffer instead of a real swapchain image
    *pImageIndex = replacementBackBufferIndex;
    replacementBackBufferIndex = (replacementBackBufferIndex + 1) % swapchainImageCount;

    // Signal the provided semaphore/fence immediately since the replacement buffer is always "available"
    if (fence != VK_NULL_HANDLE)
    {
        // Submit an empty command buffer to signal the fence
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vkQueueSubmit(gameQueue, 1, &submitInfo, fence);
    }

    if (semaphore != VK_NULL_HANDLE)
    {
        // Submit empty work with signal semaphore
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &semaphore;
        vkQueueSubmit(gameQueue, 1, &submitInfo, VK_NULL_HANDLE);
    }

    return VK_SUCCESS;
}

VkResult FrameInterpolationSwapchainVK::present(VkQueue queue, uint32_t waitSemaphoreCount,
                                                 const VkSemaphore* pWaitSemaphores)
{
    presentCount++;

    // Update config
    {
        std::lock_guard<std::mutex> lock(configMutex);

        interpolationEnabled    = currentConfig.frameGenerationEnabled;
        presentCallback         = currentConfig.presentCallback;
        presentCallbackContext  = currentConfig.presentCallbackContext;
        frameGenerationCallback = currentConfig.frameGenerationCallback;
        frameGenerationCallbackContext = currentConfig.frameGenerationCallbackContext;
        allowAsyncWorkloads     = currentConfig.allowAsyncWorkloads;
        presentInterpolatedOnly = currentConfig.onlyPresentInterpolated;
        drawDebugPacingLines    = currentConfig.drawDebugPacingLines;
        currentFrameID          = currentConfig.frameID;
    }

    if (!interpolationEnabled || !frameGenerationCallback)
    {
        // Passthrough: blit current replacement buffer to the real swapchain
        presentPassthrough();
        return VK_SUCCESS;
    }

    // Frame generation is enabled - dispatch interpolation and schedule presents

    // Get interpolation output resources
    int interpIdx = interpolationOutputIndex;
    interpolationOutputIndex = (interpolationOutputIndex + 1) % FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_INTERPOLATION_OUTPUTS_COUNT;

    // Get the current back buffer the game just rendered to
    uint32_t currentBackBufferIdx = (replacementBackBufferIndex + swapchainImageCount - 1) % swapchainImageCount;
    auto& currentBackBuffer = replacementBackBuffers[currentBackBufferIdx];

    FfxApiResource interpolatedRes = ffxApiGetResourceVK(
        interpolationOutputs[interpIdx].image,
        swapchainFormat,
        swapchainWidth, swapchainHeight,
        FFX_API_RESOURCE_STATE_UNORDERED_ACCESS,
        FFX_API_RESOURCE_USAGE_UAV);

    FfxApiResource realFrameRes = ffxApiGetResourceVK(
        currentBackBuffer.image,
        swapchainFormat,
        swapchainWidth, swapchainHeight,
        FFX_API_RESOURCE_STATE_COMPUTE_READ);

    // Dispatch frame generation
    dispatchInterpolationCommands(&interpolatedRes, &realFrameRes);

    // Schedule presents
    presentInterpolated();

    return VK_SUCCESS;
}

void FrameInterpolationSwapchainVK::presentPassthrough()
{
    // Acquire a real swapchain image
    uint32_t imageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(device, realSwapchain, UINT64_MAX,
                                             imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        return;

    uint32_t currentBackBufferIdx = (replacementBackBufferIndex + swapchainImageCount - 1) % swapchainImageCount;
    auto& currentBackBuffer = replacementBackBuffers[currentBackBufferIdx];

    // Blit replacement buffer to real swapchain image
    VkFence cmdFence;
    VkCommandBuffer cmdBuf = presentCommandPool.acquire(&cmdFence);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmdBuf, &beginInfo);

    // If there's a present callback (for UI composition), invoke it
    if (presentCallback)
    {
        FfxApiResource swapChainRes = ffxApiGetResourceVK(
            swapchainImages[imageIndex], swapchainFormat,
            swapchainWidth, swapchainHeight,
            FFX_API_RESOURCE_STATE_RENDER_TARGET,
            FFX_API_RESOURCE_USAGE_RENDERTARGET);

        FfxApiResource backBufferRes = ffxApiGetResourceVK(
            currentBackBuffer.image, swapchainFormat,
            swapchainWidth, swapchainHeight,
            FFX_API_RESOURCE_STATE_PIXEL_READ);

        ffxCallbackDescFrameGenerationPresent presentDesc{};
        presentDesc.header.type = FFX_API_CALLBACK_DESC_TYPE_FRAMEGENERATION_PRESENT;
        presentDesc.device = device;
        presentDesc.commandList = cmdBuf;
        presentDesc.currentBackBuffer = backBufferRes;
        presentDesc.currentUI = currentUiResource;
        presentDesc.outputSwapChainBuffer = swapChainRes;
        presentDesc.isGeneratedFrame = false;
        presentDesc.frameID = currentFrameID;

        presentCallback(&presentDesc, presentCallbackContext);
    }
    else
    {
        // Simple blit
        blitImage(cmdBuf, currentBackBuffer.image, swapchainImages[imageIndex],
                  VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_UNDEFINED,
                  swapchainWidth, swapchainHeight);
    }

    // Transition swapchain image to present
    transitionImageLayout(cmdBuf, swapchainImages[imageIndex],
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                          VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                          VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

    vkEndCommandBuffer(cmdBuf);

    // Submit with wait on imageAvailable, signal renderFinished
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkSubmitInfo submitInfo{};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = &imageAvailableSemaphore;
    submitInfo.pWaitDstStageMask    = &waitStage;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &cmdBuf;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &renderFinishedSemaphore;

    vkQueueSubmit(presentQueue, 1, &submitInfo, cmdFence);

    // Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &renderFinishedSemaphore;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &realSwapchain;
    presentInfo.pImageIndices      = &imageIndex;

    vkQueuePresentKHR(presentQueue, &presentInfo);
    framesSentForPresentation++;
}

void FrameInterpolationSwapchainVK::dispatchInterpolationCommands(FfxApiResource* pInterpolatedFrame, FfxApiResource* pRealFrame)
{
    if (!frameGenerationCallback)
        return;

    // Dispatch frame generation via the registered callback
    ffxDispatchDescFrameGeneration dispatchDesc{};
    dispatchDesc.header.type = FFX_API_DISPATCH_DESC_TYPE_FRAMEGENERATION;
    dispatchDesc.presentColor = *pRealFrame;
    dispatchDesc.outputs[0] = *pInterpolatedFrame;
    dispatchDesc.numGeneratedFrames = 1;
    dispatchDesc.reset = false;
    dispatchDesc.frameID = currentFrameID;

    frameGenerationCallback(&dispatchDesc, frameGenerationCallbackContext);
}

void FrameInterpolationSwapchainVK::presentInterpolated()
{
    // Present interpolated frame first, then real frame
    // This is the core frame generation presentation sequence

    uint32_t currentBackBufferIdx = (replacementBackBufferIndex + swapchainImageCount - 1) % swapchainImageCount;
    auto& currentBackBuffer = replacementBackBuffers[currentBackBufferIdx];
    int interpIdx = (interpolationOutputIndex + FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_INTERPOLATION_OUTPUTS_COUNT - 1) % FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_INTERPOLATION_OUTPUTS_COUNT;

    // --- Present the interpolated frame ---
    {
        uint32_t imageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(device, realSwapchain, UINT64_MAX,
                                                 imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            return;

        VkFence cmdFence;
        VkCommandBuffer cmdBuf = presentCommandPool.acquire(&cmdFence);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cmdBuf, &beginInfo);

        if (presentCallback)
        {
            FfxApiResource swapChainRes = ffxApiGetResourceVK(
                swapchainImages[imageIndex], swapchainFormat,
                swapchainWidth, swapchainHeight,
                FFX_API_RESOURCE_STATE_RENDER_TARGET, FFX_API_RESOURCE_USAGE_RENDERTARGET);

            FfxApiResource interpRes = ffxApiGetResourceVK(
                interpolationOutputs[interpIdx].image, swapchainFormat,
                swapchainWidth, swapchainHeight,
                FFX_API_RESOURCE_STATE_PIXEL_READ);

            ffxCallbackDescFrameGenerationPresent presentDesc{};
            presentDesc.header.type = FFX_API_CALLBACK_DESC_TYPE_FRAMEGENERATION_PRESENT;
            presentDesc.device = device;
            presentDesc.commandList = cmdBuf;
            presentDesc.currentBackBuffer = interpRes;
            presentDesc.currentUI = currentUiResource;
            presentDesc.outputSwapChainBuffer = swapChainRes;
            presentDesc.isGeneratedFrame = true;
            presentDesc.frameID = currentFrameID;

            presentCallback(&presentDesc, presentCallbackContext);
        }
        else
        {
            blitImage(cmdBuf, interpolationOutputs[interpIdx].image, swapchainImages[imageIndex],
                      VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_UNDEFINED,
                      swapchainWidth, swapchainHeight);
        }

        transitionImageLayout(cmdBuf, swapchainImages[imageIndex],
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                              VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                              VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

        vkEndCommandBuffer(cmdBuf);

        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkSubmitInfo submitInfo{};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = &imageAvailableSemaphore;
        submitInfo.pWaitDstStageMask    = &waitStage;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &cmdBuf;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = &renderFinishedSemaphore;

        vkQueueSubmit(presentQueue, 1, &submitInfo, cmdFence);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = &renderFinishedSemaphore;
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = &realSwapchain;
        presentInfo.pImageIndices      = &imageIndex;

        vkQueuePresentKHR(presentQueue, &presentInfo);
        framesSentForPresentation++;
    }

    // --- Present the real frame ---
    if (!presentInterpolatedOnly)
    {
        uint32_t imageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(device, realSwapchain, UINT64_MAX,
                                                 imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            return;

        VkFence cmdFence;
        VkCommandBuffer cmdBuf = presentCommandPool.acquire(&cmdFence);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cmdBuf, &beginInfo);

        if (presentCallback)
        {
            FfxApiResource swapChainRes = ffxApiGetResourceVK(
                swapchainImages[imageIndex], swapchainFormat,
                swapchainWidth, swapchainHeight,
                FFX_API_RESOURCE_STATE_RENDER_TARGET, FFX_API_RESOURCE_USAGE_RENDERTARGET);

            FfxApiResource backBufferRes = ffxApiGetResourceVK(
                currentBackBuffer.image, swapchainFormat,
                swapchainWidth, swapchainHeight,
                FFX_API_RESOURCE_STATE_PIXEL_READ);

            ffxCallbackDescFrameGenerationPresent presentDesc{};
            presentDesc.header.type = FFX_API_CALLBACK_DESC_TYPE_FRAMEGENERATION_PRESENT;
            presentDesc.device = device;
            presentDesc.commandList = cmdBuf;
            presentDesc.currentBackBuffer = backBufferRes;
            presentDesc.currentUI = currentUiResource;
            presentDesc.outputSwapChainBuffer = swapChainRes;
            presentDesc.isGeneratedFrame = false;
            presentDesc.frameID = currentFrameID;

            presentCallback(&presentDesc, presentCallbackContext);
        }
        else
        {
            blitImage(cmdBuf, currentBackBuffer.image, swapchainImages[imageIndex],
                      VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_UNDEFINED,
                      swapchainWidth, swapchainHeight);
        }

        transitionImageLayout(cmdBuf, swapchainImages[imageIndex],
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                              VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                              VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

        vkEndCommandBuffer(cmdBuf);

        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkSubmitInfo submitInfo{};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = &imageAvailableSemaphore;
        submitInfo.pWaitDstStageMask    = &waitStage;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &cmdBuf;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = &renderFinishedSemaphore;

        vkQueueSubmit(presentQueue, 1, &submitInfo, cmdFence);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = &renderFinishedSemaphore;
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = &realSwapchain;
        presentInfo.pImageIndices      = &imageIndex;

        vkQueuePresentKHR(presentQueue, &presentInfo);
        framesSentForPresentation++;
    }
}

void FrameInterpolationSwapchainVK::setFrameGenerationConfig(FfxFrameGenerationConfig const* config)
{
    std::lock_guard<std::mutex> lock(configMutex);
    currentConfig = *config;
}

bool FrameInterpolationSwapchainVK::waitForPresents()
{
    if (device != VK_NULL_HANDLE)
        vkDeviceWaitIdle(device);
    return true;
}

FfxApiResource FrameInterpolationSwapchainVK::interpolationOutput(int index)
{
    if (index < 0 || index >= FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_INTERPOLATION_OUTPUTS_COUNT)
        index = 0;

    return ffxApiGetResourceVK(
        interpolationOutputs[index].image,
        swapchainFormat,
        swapchainWidth, swapchainHeight,
        FFX_API_RESOURCE_STATE_UNORDERED_ACCESS,
        FFX_API_RESOURCE_USAGE_UAV);
}

VkCommandBuffer FrameInterpolationSwapchainVK::getInterpolationCommandBuffer()
{
    VkCommandBuffer cmdBuf = interpolationCommandPool.acquire();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmdBuf, &beginInfo);

    return cmdBuf;
}

void FrameInterpolationSwapchainVK::registerUiResource(FfxApiResource uiResource, uint32_t flags)
{
    currentUiResource = uiResource;
    uiCompositionFlags = flags;

    if (uiResource.resource && (flags & FFX_FRAMEGENERATION_UI_COMPOSITION_FLAG_ENABLE_INTERNAL_UI_DOUBLE_BUFFERING))
    {
        if (uiReplacementBuffer.image == VK_NULL_HANDLE)
        {
            VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            uiReplacementBuffer.create(device, physicalDevice,
                                       uiResource.description.width, uiResource.description.height,
                                       ffxGetVKFormatFromSurfaceFormat((FfxApiSurfaceFormat)uiResource.description.format),
                                       usage, gameQueueFamilyIndex);
        }
    }
}

void FrameInterpolationSwapchainVK::setWaitCallback(FfxWaitCallbackFunc func)
{
    waitCallback = func;
}

void FrameInterpolationSwapchainVK::setFramePacingTuning(const FfxApiSwapchainFramePacingTuningVK* tuning)
{
    if (tuning)
    {
        safetyMarginInSec = tuning->safetyMarginInSec;
        varianceFactor    = tuning->varianceFactor;
        allowHybridSpin   = tuning->allowHybridSpin;
        hybridSpinTime    = tuning->hybridSpinTime;
    }
}

void FrameInterpolationSwapchainVK::getGpuMemoryUsage(FfxApiEffectMemoryUsage* vramUsage)
{
    if (vramUsage)
    {
        vramUsage->totalUsageInBytes     = totalUsageInBytes;
        vramUsage->aliasableUsageInBytes = aliasableUsageInBytes;
    }
}

bool FrameInterpolationSwapchainVK::spawnPresenterThread()
{
    if (presenterThread.joinable())
        return true;

    presenterShutdown = false;
    presenterThread = std::thread(&FrameInterpolationSwapchainVK::presenterThreadFunc, this);
    return true;
}

bool FrameInterpolationSwapchainVK::killPresenterThread()
{
    if (!presenterThread.joinable())
        return true;

    presenterShutdown = true;
    presenterCondition.notify_all();
    presenterThread.join();
    return true;
}

void FrameInterpolationSwapchainVK::presenterThreadFunc()
{
    SimpleMovingAverage<16> frameTimeAvg;

    while (!presenterShutdown)
    {
        FrameInterpolationPacingDataVK localPresents;
        {
            std::unique_lock<std::mutex> lock(presenterMutex);
            presenterCondition.wait(lock, [this] {
                return presenterShutdown || scheduledPresents.numFramesToPresent > 0;
            });

            if (presenterShutdown)
                break;

            localPresents = scheduledPresents;
            scheduledPresents.invalidate();
        }

        if (localPresents.numFramesToPresent == 0)
            continue;

        // Frame pacing logic
        int64_t nowNs = static_cast<int64_t>(getTimeNanoseconds());
        if (previousPresentNs > 0)
        {
            double frameDeltaSec = static_cast<double>(nowNs - previousPresentNs) / 1e9;
            frameTimeAvg.update(frameDeltaSec);
        }

        for (uint32_t i = 0; i < localPresents.numFramesToPresent && i < FrameTypeVK_Count; ++i)
        {
            auto& frame = localPresents.frames[i];
            if (!frame.doPresent)
                continue;

            // Apply frame pacing: wait until target time
            if (frame.presentTimeDeltaNs > 0 && previousPresentNs > 0)
            {
                int64_t targetNs = previousPresentNs + frame.presentTimeDeltaNs;
                waitForPerformanceCount(targetNs);
            }

            previousPresentNs = static_cast<int64_t>(getTimeNanoseconds());
        }
    }
}

void FrameInterpolationSwapchainVK::discardOutstandingInterpolationCommandLists()
{
    // No-op in current implementation - command buffers are managed by the pool
}

// ---------------------------------------------------------------------------
// Free function API implementations (called from ffx_vk.h)
// ---------------------------------------------------------------------------

FfxSwapchain ffxGetSwapchainVK(VkSwapchainKHR swapchain)
{
    return reinterpret_cast<FfxSwapchain>(swapchain);
}

VkSwapchainKHR ffxGetVKSwapchain(FfxSwapchain ffxSwapchain)
{
    return reinterpret_cast<VkSwapchainKHR>(ffxSwapchain);
}

FfxErrorCode ffxReplaceSwapchainForFrameinterpolationVK(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkQueue gameQueue,
    uint32_t gameQueueFamilyIndex,
    VkSwapchainKHR* swapchain,
    VkSurfaceKHR surface)
{
    if (!swapchain || *swapchain == VK_NULL_HANDLE)
        return FFX_ERROR_INVALID_ARGUMENT;

    auto* proxy = new FrameInterpolationSwapchainVK();
    FfxErrorCode result = proxy->init(device, physicalDevice, gameQueue, gameQueueFamilyIndex, *swapchain, surface);

    if (result != FFX_OK)
    {
        delete proxy;
        return result;
    }

    ffxRegisterFrameInterpolationSwapchainVK(*swapchain, proxy);
    return FFX_OK;
}

FfxErrorCode ffxWaitForPresentsVK(VkSwapchainKHR swapchain)
{
    auto* proxy = ffxGetFrameInterpolationSwapchainVK(swapchain);
    if (!proxy)
        return FFX_ERROR_INVALID_ARGUMENT;
    proxy->waitForPresents();
    return FFX_OK;
}

FfxErrorCode ffxRegisterFrameinterpolationUiResourceVK(VkSwapchainKHR swapchain, FfxApiResource uiResource, uint32_t flags)
{
    auto* proxy = ffxGetFrameInterpolationSwapchainVK(swapchain);
    if (!proxy)
        return FFX_ERROR_INVALID_ARGUMENT;
    proxy->registerUiResource(uiResource, flags);
    return FFX_OK;
}

FfxErrorCode ffxGetFrameinterpolationCommandlistVK(VkSwapchainKHR swapchain, FfxCommandList& commandList)
{
    auto* proxy = ffxGetFrameInterpolationSwapchainVK(swapchain);
    if (!proxy)
        return FFX_ERROR_INVALID_ARGUMENT;
    commandList = reinterpret_cast<FfxCommandList>(proxy->getInterpolationCommandBuffer());
    return FFX_OK;
}

FfxApiResource ffxGetFrameinterpolationTextureVK(VkSwapchainKHR swapchain)
{
    auto* proxy = ffxGetFrameInterpolationSwapchainVK(swapchain);
    if (!proxy)
    {
        FfxApiResource empty{};
        return empty;
    }
    return proxy->interpolationOutput();
}

FfxErrorCode ffxSetFrameGenerationConfigToSwapchainVK(FfxFrameGenerationConfig const* config)
{
    if (!config || !config->swapChain)
        return FFX_ERROR_INVALID_ARGUMENT;

    VkSwapchainKHR swapchain = reinterpret_cast<VkSwapchainKHR>(config->swapChain);
    auto* proxy = ffxGetFrameInterpolationSwapchainVK(swapchain);
    if (!proxy)
        return FFX_ERROR_INVALID_ARGUMENT;

    proxy->setFrameGenerationConfig(config);
    return FFX_OK;
}

FfxErrorCode ffxFrameInterpolationSwapchainGetGpuMemoryUsageVK(VkSwapchainKHR swapchain, FfxApiEffectMemoryUsage* vramUsage)
{
    auto* proxy = ffxGetFrameInterpolationSwapchainVK(swapchain);
    if (!proxy || !vramUsage)
        return FFX_ERROR_INVALID_ARGUMENT;
    proxy->getGpuMemoryUsage(vramUsage);
    return FFX_OK;
}

FfxErrorCode ffxConfigureFrameInterpolationSwapchainVK(VkSwapchainKHR swapchain, FfxFrameInterpolationSwapchainConfigureKeyVK key, void* valuePtr)
{
    auto* proxy = ffxGetFrameInterpolationSwapchainVK(swapchain);
    if (!proxy)
        return FFX_ERROR_INVALID_ARGUMENT;

    switch (key)
    {
    case FFX_FI_SWAPCHAIN_VK_CONFIGURE_KEY_WAITCALLBACK:
        proxy->setWaitCallback(reinterpret_cast<FfxWaitCallbackFunc>(valuePtr));
        break;
    case FFX_FI_SWAPCHAIN_VK_CONFIGURE_KEY_FRAMEPACINGTUNING:
        proxy->setFramePacingTuning(reinterpret_cast<const FfxApiSwapchainFramePacingTuningVK*>(valuePtr));
        break;
    default:
        return FFX_ERROR_INVALID_ARGUMENT;
    }
    return FFX_OK;
}

// Additional backend functions
FfxApiSurfaceFormat ffxGetSurfaceFormatVK(VkFormat format)
{
    return static_cast<FfxApiSurfaceFormat>(ffxApiGetSurfaceFormatVK(format));
}

VkFormat ffxGetVKFormat(FfxApiSurfaceFormat surfaceFormat)
{
    return ffxGetVKFormatFromSurfaceFormat(surfaceFormat);
}

FfxApiResourceDescription ffxGetResourceDescriptionVK(VkImage image, VkFormat format, uint32_t width, uint32_t height, FfxApiResourceUsage additionalUsages)
{
    FfxApiResourceDescription desc{};
    desc.type     = FFX_API_RESOURCE_TYPE_TEXTURE2D;
    desc.format   = ffxApiGetSurfaceFormatVK(format);
    desc.width    = width;
    desc.height   = height;
    desc.depth    = 1;
    desc.mipCount = 1;
    desc.flags    = FFX_API_RESOURCE_FLAGS_NONE;
    desc.usage    = FFX_API_RESOURCE_USAGE_READ_ONLY | additionalUsages;
    return desc;
}

FfxDevice ffxGetDeviceVK(VkDevice device)
{
    return reinterpret_cast<FfxDevice>(device);
}

FfxCommandList ffxGetCommandListVK(VkCommandBuffer cmdBuffer)
{
    return reinterpret_cast<FfxCommandList>(cmdBuffer);
}

FfxApiResource ffxGetResourceVK(VkImage image, FfxApiResourceDescription ffxResDescription, const wchar_t* ffxResName, uint32_t state)
{
    (void)ffxResName;
    FfxApiResource res{};
    res.resource = reinterpret_cast<void*>(image);
    res.description = ffxResDescription;
    res.state = state;
    return res;
}
