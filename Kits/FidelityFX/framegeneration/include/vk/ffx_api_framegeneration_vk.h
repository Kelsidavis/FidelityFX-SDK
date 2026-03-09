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
#include "../../../api/include/vk/ffx_api_vk.h"

#define FFX_FRAMEGENERATION_SWAPCHAIN_VK_VERSION_MAJOR 3
#define FFX_FRAMEGENERATION_SWAPCHAIN_VK_VERSION_MINOR 1
#define FFX_FRAMEGENERATION_SWAPCHAIN_VK_VERSION_PATCH 6

#define FFX_FRAMEGENERATION_SWAPCHAIN_VK_MAKE_VERSION(major, minor, patch) (((major) << 22) | ((minor) << 12) | (patch))
#define FFX_FRAMEGENERATION_SWAPCHAIN_VK_VERSION FFX_FRAMEGENERATION_SWAPCHAIN_VK_MAKE_VERSION(FFX_FRAMEGENERATION_SWAPCHAIN_VK_VERSION_MAJOR, FFX_FRAMEGENERATION_SWAPCHAIN_VK_VERSION_MINOR, FFX_FRAMEGENERATION_SWAPCHAIN_VK_VERSION_PATCH)

/// Descriptor to create a frame generation swapchain by wrapping an existing VkSwapchainKHR.
#define FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_WRAP_VK FFX_API_MAKE_BACKEND_EFFECT_SUB_ID(FFX_API_BACKEND_ID_VK, FFX_API_EFFECT_ID_FRAMEGENERATIONSWAPCHAIN, 0x01)
struct ffxCreateContextDescFrameGenerationSwapChainWrapVK
{
    ffxCreateContextDescHeader header;
    VkSwapchainKHR*    swapchain;           ///< Input swap chain to wrap, output frame interpolation swapchain.
    VkDevice           device;              ///< Vulkan logical device.
    VkPhysicalDevice   physicalDevice;      ///< Vulkan physical device.
    VkQueue            gameQueue;           ///< Input queue to be used for game rendering.
    uint32_t           gameQueueFamilyIndex;///< Queue family index of gameQueue.
    VkSurfaceKHR       surface;             ///< Surface for presentation.
};

/// Descriptor to create a new VkSwapchainKHR with frame interpolation support.
#define FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_NEW_VK FFX_API_MAKE_BACKEND_EFFECT_SUB_ID(FFX_API_BACKEND_ID_VK, FFX_API_EFFECT_ID_FRAMEGENERATIONSWAPCHAIN, 0x05)
struct ffxCreateContextDescFrameGenerationSwapChainNewVK
{
    ffxCreateContextDescHeader header;
    VkSwapchainKHR*            swapchain;           ///< Output frame interpolation swapchain.
    VkSwapchainCreateInfoKHR*  createInfo;           ///< Swap chain creation parameters.
    VkDevice                   device;               ///< Vulkan logical device.
    VkPhysicalDevice           physicalDevice;       ///< Vulkan physical device.
    VkQueue                    gameQueue;             ///< Input queue to be used for game rendering.
    uint32_t                   gameQueueFamilyIndex;  ///< Queue family index of gameQueue.
};

/// Descriptor to register a UI resource with the VK frame interpolation swapchain.
#define FFX_API_CONFIGURE_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_REGISTERUIRESOURCE_VK FFX_API_MAKE_BACKEND_EFFECT_SUB_ID(FFX_API_BACKEND_ID_VK, FFX_API_EFFECT_ID_FRAMEGENERATIONSWAPCHAIN, 0x02)
struct ffxConfigureDescFrameGenerationSwapChainRegisterUiResourceVK
{
    ffxConfigureDescHeader header;
    struct FfxApiResource  uiResource;   ///< Resource containing user interface for composition. May be empty.
    uint32_t               flags;        ///< Zero or combination of values from FfxApiUiCompositionFlags.
};

/// Descriptor to query the interpolation command list from the VK frame interpolation swapchain.
#define FFX_API_QUERY_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_INTERPOLATIONCOMMANDLIST_VK FFX_API_MAKE_BACKEND_EFFECT_SUB_ID(FFX_API_BACKEND_ID_VK, FFX_API_EFFECT_ID_FRAMEGENERATIONSWAPCHAIN, 0x03)
struct ffxQueryDescFrameGenerationSwapChainInterpolationCommandListVK
{
    ffxQueryDescHeader header;
    void** pOutCommandList;             ///< Output command list (VkCommandBuffer) to be used for frame generation dispatch.
};

/// Descriptor to query the interpolation texture from the VK frame interpolation swapchain.
#define FFX_API_QUERY_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_INTERPOLATIONTEXTURE_VK FFX_API_MAKE_BACKEND_EFFECT_SUB_ID(FFX_API_BACKEND_ID_VK, FFX_API_EFFECT_ID_FRAMEGENERATIONSWAPCHAIN, 0x04)
struct ffxQueryDescFrameGenerationSwapChainInterpolationTextureVK
{
    ffxQueryDescHeader header;
    struct FfxApiResource *pOutTexture; ///< Output resource in which the frame interpolation result should be placed.
};

/// Descriptor to wait for all pending presents to complete.
#define FFX_API_DISPATCH_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_WAIT_FOR_PRESENTS_VK FFX_API_MAKE_BACKEND_EFFECT_SUB_ID(FFX_API_BACKEND_ID_VK, FFX_API_EFFECT_ID_FRAMEGENERATIONSWAPCHAIN, 0x07)
struct ffxDispatchDescFrameGenerationSwapChainWaitForPresentsVK
{
    ffxDispatchDescHeader header;
};

/// Descriptor for configuring frame generation swapchain via KeyValue API.
#define FFX_API_CONFIGURE_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_KEYVALUE_VK FFX_API_MAKE_BACKEND_EFFECT_SUB_ID(FFX_API_BACKEND_ID_VK, FFX_API_EFFECT_ID_FRAMEGENERATIONSWAPCHAIN, 0x08)
struct ffxConfigureDescFrameGenerationSwapChainKeyValueVK
{
    ffxConfigureDescHeader  header;
    uint64_t                key;        ///< Configuration key.
    uint64_t                u64;        ///< Integer value or enum value to set.
    void*                   ptr;        ///< Pointer to set or pointer to value to set.
};

enum FfxApiConfigureFrameGenerationSwapChainKeyVK
{
    FFX_API_CONFIGURE_FG_SWAPCHAIN_KEY_VK_WAITCALLBACK = 0,
    FFX_API_CONFIGURE_FG_SWAPCHAIN_KEY_VK_FRAMEPACINGTUNING = 2,
};

/// Descriptor to query GPU memory usage of the VK frame interpolation swapchain.
#define FFX_API_QUERY_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_GPU_MEMORY_USAGE_VK FFX_API_MAKE_BACKEND_EFFECT_SUB_ID(FFX_API_BACKEND_ID_VK, FFX_API_EFFECT_ID_FRAMEGENERATIONSWAPCHAIN, 0x09)
struct ffxQueryFrameGenerationSwapChainGetGPUMemoryUsageVK
{
    ffxQueryDescHeader header;
    struct FfxApiEffectMemoryUsage* gpuMemoryUsageFrameGenerationSwapchain;
};

/// Version descriptor for VK frame generation swapchain.
#define FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_VERSION_VK FFX_API_MAKE_BACKEND_EFFECT_SUB_ID(FFX_API_BACKEND_ID_VK, FFX_API_EFFECT_ID_FRAMEGENERATIONSWAPCHAIN, 0x0b)
struct ffxCreateContextDescFrameGenerationSwapChainVersionVK
{
    ffxCreateContextDescHeader header;
    uint32_t                   version;           ///< The version of the API the application was built against.
};
