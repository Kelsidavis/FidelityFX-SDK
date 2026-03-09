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

#pragma once

#include <vulkan/vulkan.h>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "FrameInterpolationSwapchainVK_Helpers.h"
#include "../../include/ffx_framegeneration.h"
#include "../../internal/ffx_framegeneration_internal.h"
#include "../../fsr3/include/ffx_frameinterpolation.h"
#include "../../../backend/vk/ffx_vk.h"

#define FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_MAX_IMAGE_COUNT    8
#define FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_INTERPOLATION_OUTPUTS_COUNT 2
#define FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_COMMAND_POOL_SIZE  32

typedef enum FrameTypeVK
{
    FrameTypeVK_Interpolated_1,
    FrameTypeVK_Real,
    FrameTypeVK_Count
} FrameTypeVK;

struct FrameInterpolationFrameInfoVK
{
    bool            doPresent;
    FfxApiResource  resource;
    uint64_t        interpolationCompletedSemaphoreValue;
    uint64_t        presentIndex;
    int64_t         presentTimeDeltaNs;
};

struct FrameInterpolationPacingDataVK
{
    FfxApiPresentCallbackFunc       presentCallback = nullptr;
    void*                           presentCallbackContext = nullptr;
    FfxApiResource                  uiSurface;

    bool                            vsync;
    bool                            usePremulAlphaComposite;
    bool                            drawDebugPacingLines;

    uint64_t                        interpolationCompletedSemaphoreValue;
    uint64_t                        replacementBufferSemaphoreSignal;
    uint64_t                        numFramesSentForPresentationBase;
    uint32_t                        numFramesToPresent;
    uint64_t                        currentFrameID;

    FrameInterpolationFrameInfoVK   frames[FrameTypeVK_Count];

    void invalidate() { memset(this, 0, sizeof(*this)); }
};

/// Replacement resource backed by a VkImage with its own memory allocation.
struct ReplacementResourceVK
{
    VkImage                 image       = VK_NULL_HANDLE;
    VkDeviceMemory          memory      = VK_NULL_HANDLE;
    VkImageView             view        = VK_NULL_HANDLE;
    uint64_t                availabilitySemaphoreValue = 0;
    uint32_t                width       = 0;
    uint32_t                height      = 0;
    VkFormat                format      = VK_FORMAT_UNDEFINED;

    void destroy(VkDevice device);
    bool create(VkDevice device, VkPhysicalDevice physDevice, uint32_t w, uint32_t h, VkFormat fmt, VkImageUsageFlags usage, uint32_t queueFamilyIndex);
};

/// The core Vulkan frame interpolation swapchain proxy.
///
/// This object wraps a real VkSwapchainKHR and intercepts present calls to
/// insert interpolated frames, managing multi-queue synchronization via
/// timeline semaphores and a dedicated presenter thread for frame pacing.
class FrameInterpolationSwapchainVK
{
public:
    FrameInterpolationSwapchainVK();
    ~FrameInterpolationSwapchainVK();

    /// Initialize by wrapping an existing swapchain.
    FfxErrorCode init(VkDevice device, VkPhysicalDevice physDevice,
                      VkQueue gameQueue, uint32_t gameQueueFamilyIndex,
                      VkSwapchainKHR realSwapchain, VkSurfaceKHR surface);

    /// Initialize by creating a new swapchain from creation parameters.
    FfxErrorCode initNew(VkDevice device, VkPhysicalDevice physDevice,
                         VkQueue gameQueue, uint32_t gameQueueFamilyIndex,
                         const VkSwapchainCreateInfoKHR* createInfo);

    void shutdown();

    // --- Public API (mapped from ffx_vk.h functions) ---

    void setFrameGenerationConfig(FfxFrameGenerationConfig const* config);
    bool waitForPresents();

    FfxApiResource interpolationOutput(int index = 0);
    VkCommandBuffer getInterpolationCommandBuffer();

    void registerUiResource(FfxApiResource uiResource, uint32_t flags);
    void setWaitCallback(FfxWaitCallbackFunc waitCallbackFunc);
    void setFramePacingTuning(const FfxApiSwapchainFramePacingTuningVK* tuning);

    void getGpuMemoryUsage(FfxApiEffectMemoryUsage* vramUsage);

    /// Called by the proxy "vkQueuePresentKHR" replacement.
    VkResult present(VkQueue queue, uint32_t waitSemaphoreCount,
                     const VkSemaphore* pWaitSemaphores);

    /// Called by the proxy "vkAcquireNextImageKHR" replacement.
    VkResult acquireNextImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex);

    /// Get the replacement back buffer image for a given index.
    VkImage getBackBufferImage(uint32_t index) const;
    VkFormat getSwapchainFormat() const { return swapchainFormat; }
    uint32_t getSwapchainWidth() const { return swapchainWidth; }
    uint32_t getSwapchainHeight() const { return swapchainHeight; }

    VkSwapchainKHR getRealSwapchain() const { return realSwapchain; }

private:
    // Vulkan handles
    VkDevice            device              = VK_NULL_HANDLE;
    VkPhysicalDevice    physicalDevice      = VK_NULL_HANDLE;
    VkSwapchainKHR      realSwapchain       = VK_NULL_HANDLE;
    VkSurfaceKHR        surface             = VK_NULL_HANDLE;
    VkQueue             gameQueue           = VK_NULL_HANDLE;
    VkQueue             presentQueue        = VK_NULL_HANDLE;
    VkQueue             interpolationQueue  = VK_NULL_HANDLE;
    uint32_t            gameQueueFamilyIndex      = 0;
    uint32_t            presentQueueFamilyIndex   = 0;

    // Swapchain properties
    VkFormat            swapchainFormat     = VK_FORMAT_UNDEFINED;
    uint32_t            swapchainWidth      = 0;
    uint32_t            swapchainHeight     = 0;
    uint32_t            swapchainImageCount = 0;
    VkImage             swapchainImages[FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_MAX_IMAGE_COUNT] = {};

    // Replacement back buffers (what the game renders to)
    ReplacementResourceVK   replacementBackBuffers[FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_MAX_IMAGE_COUNT] = {};
    int                     replacementBackBufferIndex = 0;

    // Interpolation output images
    ReplacementResourceVK   interpolationOutputs[FFX_FRAME_INTERPOLATION_SWAP_CHAIN_VK_INTERPOLATION_OUTPUTS_COUNT] = {};
    int                     interpolationOutputIndex = 0;

    // UI replacement buffer
    ReplacementResourceVK   uiReplacementBuffer = {};
    FfxApiResource          currentUiResource = {};
    uint32_t                uiCompositionFlags = 0;

    // Synchronization primitives
    VkSemaphore             gameSemaphore           = VK_NULL_HANDLE;  // timeline
    VkSemaphore             interpolationSemaphore  = VK_NULL_HANDLE;  // timeline
    VkSemaphore             presentSemaphore        = VK_NULL_HANDLE;  // timeline
    VkSemaphore             replacementBufferSemaphore = VK_NULL_HANDLE; // timeline
    VkSemaphore             imageAvailableSemaphore = VK_NULL_HANDLE;  // binary, for vkAcquireNextImageKHR
    VkSemaphore             renderFinishedSemaphore = VK_NULL_HANDLE;  // binary, for present

    uint64_t                gameSemaphoreValue              = 0;
    uint64_t                interpolationSemaphoreValue     = 0;
    uint64_t                presentSemaphoreValue           = 0;

    // Command pools
    VkCommandPool_          presentCommandPool;
    VkCommandPool_          interpolationCommandPool;

    // Presenter thread
    std::thread             presenterThread;
    std::mutex              presenterMutex;
    std::condition_variable presenterCondition;
    volatile bool           presenterShutdown       = false;
    volatile bool           presenterResetTimer     = false;

    // Pacing data (double-buffered via mutex)
    FrameInterpolationPacingDataVK  scheduledInterpolations;
    FrameInterpolationPacingDataVK  scheduledPresents;
    std::mutex                      scheduledFrameMutex;

    // Frame generation config
    FfxFrameGenerationConfig        currentConfig = {};
    std::mutex                      configMutex;

    // State
    bool                interpolationEnabled        = false;
    bool                presentInterpolatedOnly     = false;
    bool                previousFrameWasInterpolated = false;
    bool                drawDebugPacingLines        = false;
    bool                allowAsyncWorkloads         = false;
    uint64_t            currentFrameID              = 0;
    uint64_t            framesSentForPresentation   = 0;
    uint64_t            presentCount                = 0;
    std::atomic<int>    refCount{1};

    // Frame pacing tuning
    volatile double     safetyMarginInSec       = 0.0001;
    volatile double     varianceFactor          = 0.1;
    volatile bool       allowHybridSpin         = false;
    volatile uint32_t   hybridSpinTime          = 2;
    FfxWaitCallbackFunc waitCallback            = nullptr;
    volatile int64_t    previousPresentNs       = 0;

    // Callbacks
    FfxApiPresentCallbackFunc           presentCallback             = nullptr;
    void*                               presentCallbackContext      = nullptr;
    FfxApiFrameGenerationDispatchFunc   frameGenerationCallback     = nullptr;
    void*                               frameGenerationCallbackContext = nullptr;

    // VRAM tracking
    uint64_t totalUsageInBytes      = 0;
    uint64_t aliasableUsageInBytes  = 0;

    // Internal methods
    void        createSyncPrimitives();
    void        destroySyncPrimitives();
    bool        createReplacementResources();
    void        destroyReplacementResources();
    void        presenterThreadFunc();
    void        presentPassthrough();
    void        presentWithUiComposition();
    void        dispatchInterpolationCommands(FfxApiResource* pInterpolatedFrame, FfxApiResource* pRealFrame);
    void        presentInterpolated();
    bool        spawnPresenterThread();
    bool        killPresenterThread();
    void        discardOutstandingInterpolationCommandLists();
    bool        findPresentQueue();

    /// Blit src image to dst image via a command buffer submitted on the given queue.
    void        blitImage(VkCommandBuffer cmdBuf, VkImage src, VkImage dst,
                          VkImageLayout srcLayout, VkImageLayout dstLayout,
                          uint32_t width, uint32_t height);

    /// Transition image layout with a pipeline barrier.
    static void transitionImageLayout(VkCommandBuffer cmdBuf, VkImage image,
                                      VkImageLayout oldLayout, VkImageLayout newLayout,
                                      VkAccessFlags srcAccess, VkAccessFlags dstAccess,
                                      VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage,
                                      uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED,
                                      uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED);
};

/// Global registry for looking up the proxy swapchain from a VkSwapchainKHR handle.
FrameInterpolationSwapchainVK* ffxGetFrameInterpolationSwapchainVK(VkSwapchainKHR swapchain);
void ffxRegisterFrameInterpolationSwapchainVK(VkSwapchainKHR handle, FrameInterpolationSwapchainVK* proxy);
void ffxUnregisterFrameInterpolationSwapchainVK(VkSwapchainKHR handle);
