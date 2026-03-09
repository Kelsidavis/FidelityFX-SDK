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

/// @defgroup VKBackend VK Backend
/// FidelityFX SDK native backend implementation for Vulkan.
///
/// @ingroup Backends

/// @defgroup VKFrameInterpolation VK FrameInterpolation
/// FidelityFX SDK native frame interpolation implementation for Vulkan backend.
///
/// @ingroup VKBackend

#pragma once

#include <vulkan/vulkan.h>
#include "../../api/internal/ffx_interface.h"
#include "../../api/include/vk/ffx_api_vk.h"

#if defined(__cplusplus)
extern "C" {
#endif // #if defined(__cplusplus)

/// Query how much memory is required for the Vulkan backend's scratch buffer.
///
/// @param [in] maxContexts                 The maximum number of simultaneous effect contexts that will share the backend.
///
/// @returns
/// The size (in bytes) of the required scratch memory buffer for the VK backend.
/// @ingroup VKBackend
FFX_API size_t ffxGetScratchMemorySizeVK(size_t maxContexts);

/// Create a FfxDevice from a VkDevice + VkPhysicalDevice.
///
/// @param [in] device                      The Vulkan logical device.
///
/// @returns
/// An abstract FidelityFX device.
///
/// @ingroup VKBackend
FFX_API FfxDevice ffxGetDeviceVK(VkDevice device);

/// Populate an interface with pointers for the VK backend.
///
/// @param [out] backendInterface           A pointer to a FfxInterface structure to populate with pointers.
/// @param [in] device                      The Vulkan logical device.
/// @param [in] physicalDevice              The Vulkan physical device.
/// @param [in] scratchBuffer               A pointer to a buffer of memory for the backend.
/// @param [in] scratchBufferSize           The size (in bytes) of the scratch buffer.
/// @param [in] maxContexts                 The maximum number of simultaneous effect contexts.
///
/// @retval
/// FFX_OK                                  The operation completed successfully.
/// @retval
/// FFX_ERROR_CODE_INVALID_POINTER          The interface pointer was NULL.
///
/// @ingroup VKBackend
FFX_API FfxErrorCode ffxGetInterfaceVK(
    FfxInterface* backendInterface,
    FfxDevice device,
    VkPhysicalDevice physicalDevice,
    void* scratchBuffer,
    size_t scratchBufferSize,
    size_t maxContexts);

/// Create a FfxCommandList from a VkCommandBuffer.
///
/// @param [in] cmdBuffer                   The Vulkan command buffer.
///
/// @returns
/// An abstract FidelityFX command list.
///
/// @ingroup VKBackend
FFX_API FfxCommandList ffxGetCommandListVK(VkCommandBuffer cmdBuffer);

/// Fetch a FfxApiResource from a VkImage.
///
/// @param [in] image                       The Vulkan image.
/// @param [in] ffxResDescription           An FfxApiResourceDescription for the resource representation.
/// @param [in] ffxResName                  (optional) A name string to identify the resource in debug mode.
/// @param [in] state                       The state the resource is currently in.
///
/// @returns
/// An abstract FidelityFX resource.
///
/// @ingroup VKBackend
FFX_API FfxApiResource ffxGetResourceVK(VkImage image,
                                        FfxApiResourceDescription ffxResDescription,
                                        const wchar_t* ffxResName,
                                        uint32_t state = FFX_API_RESOURCE_STATE_COMPUTE_READ);

/// Fetch a FfxApiSurfaceFormat from a VkFormat.
///
/// @param [in] format              The VkFormat to convert.
///
/// @returns
/// An FfxApiSurfaceFormat.
///
/// @ingroup VKBackend
FFX_API FfxApiSurfaceFormat ffxGetSurfaceFormatVK(VkFormat format);

/// Fetch a VkFormat from a FfxApiSurfaceFormat.
///
/// @param [in] surfaceFormat       The FfxApiSurfaceFormat to convert.
///
/// @returns
/// A VkFormat.
///
/// @ingroup VKBackend
FFX_API VkFormat ffxGetVKFormat(FfxApiSurfaceFormat surfaceFormat);

/// Fetch a FfxApiResourceDescription from an existing VkImage (requires external image info).
///
/// @param [in] image               The VkImage.
/// @param [in] format              The VkFormat of the image.
/// @param [in] width               The width of the image.
/// @param [in] height              The height of the image.
/// @param [in] additionalUsages    Optional FfxApiResourceUsage flags.
///
/// @returns
/// An FfxApiResourceDescription.
///
/// @ingroup VKBackend
FFX_API FfxApiResourceDescription ffxGetResourceDescriptionVK(VkImage image, VkFormat format, uint32_t width, uint32_t height, FfxApiResourceUsage additionalUsages = FFX_API_RESOURCE_USAGE_READ_ONLY);

/// Fetch a FfxSwapchain from an existing VkSwapchainKHR.
///
/// @param [in] swapchain           The VkSwapchainKHR to wrap.
///
/// @returns
/// An FfxSwapchain.
///
/// @ingroup VKFrameInterpolation
FFX_API FfxSwapchain ffxGetSwapchainVK(VkSwapchainKHR swapchain);

/// Fetch a VkSwapchainKHR from an existing FfxSwapchain.
///
/// @param [in] ffxSwapchain        The FfxSwapchain to unwrap.
///
/// @returns
/// A VkSwapchainKHR.
///
/// @ingroup VKFrameInterpolation
FFX_API VkSwapchainKHR ffxGetVKSwapchain(FfxSwapchain ffxSwapchain);

/// Replaces the current swapchain with the frame interpolation proxy swapchain.
///
/// @param [in] device              The VkDevice.
/// @param [in] physicalDevice      The VkPhysicalDevice.
/// @param [in] gameQueue           The VkQueue used for game rendering.
/// @param [in] gameQueueFamilyIndex The queue family index for the game queue.
/// @param [in,out] swapchain       The VkSwapchainKHR to replace.
/// @param [in] surface             The VkSurfaceKHR for presentation.
///
/// @retval
/// FFX_OK                              The operation completed successfully.
/// @retval
/// FFX_ERROR_INVALID_ARGUMENT          One of the parameters is invalid.
///
/// @ingroup VKFrameInterpolation
FFX_API FfxErrorCode ffxReplaceSwapchainForFrameinterpolationVK(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkQueue gameQueue,
    uint32_t gameQueueFamilyIndex,
    VkSwapchainKHR* swapchain,
    VkSurfaceKHR surface);

/// Waits for the FfxSwapchain to complete presentation.
///
/// @param [in] swapchain           The FfxSwapchain/VkSwapchainKHR to wait on.
///
/// @retval
/// FFX_OK                              The operation completed successfully.
/// @retval
/// FFX_ERROR_INVALID_ARGUMENT          Invalid swapchain handle.
///
/// @ingroup VKFrameInterpolation
FFX_API FfxErrorCode ffxWaitForPresentsVK(VkSwapchainKHR swapchain);

/// Registers a FfxApiResource to use for UI with the provided swapchain.
///
/// @param [in] swapchain           The VkSwapchainKHR to register with.
/// @param [in] uiResource          The FfxApiResource representing the UI resource.
/// @param [in] flags               A set of FfxApiUiCompositionFlags.
///
/// @retval
/// FFX_OK                              The operation completed successfully.
/// @retval
/// FFX_ERROR_INVALID_ARGUMENT          Invalid swapchain handle.
///
/// @ingroup VKFrameInterpolation
FFX_API FfxErrorCode ffxRegisterFrameinterpolationUiResourceVK(VkSwapchainKHR swapchain, FfxApiResource uiResource, uint32_t flags);

/// Fetches a FfxCommandList from the FfxSwapchain for recording interpolation commands.
///
/// @param [in] swapchain           The VkSwapchainKHR to get a command buffer from.
/// @param [out] commandList        The FfxCommandList (VkCommandBuffer) from the swapchain.
///
/// @retval
/// FFX_OK                              The operation completed successfully.
/// @retval
/// FFX_ERROR_INVALID_ARGUMENT          Invalid swapchain handle.
///
/// @ingroup VKFrameInterpolation
FFX_API FfxErrorCode ffxGetFrameinterpolationCommandlistVK(VkSwapchainKHR swapchain, FfxCommandList& commandList);

/// Fetches a FfxApiResource representing the interpolation output from the FfxSwapchain.
///
/// @param [in] swapchain           The VkSwapchainKHR to get the interpolation texture from.
///
/// @returns
/// An FfxApiResource for the interpolation output.
///
/// @ingroup VKFrameInterpolation
FFX_API FfxApiResource ffxGetFrameinterpolationTextureVK(VkSwapchainKHR swapchain);

/// Sets a FfxFrameGenerationConfig to the internal FrameInterpolationSwapChain.
///
/// @param [in] config                  The FfxFrameGenerationConfig to set.
///
/// @retval
/// FFX_OK                              The operation completed successfully.
/// @retval
/// FFX_ERROR_INVALID_ARGUMENT          Invalid swapchain in config.
///
/// @ingroup VKFrameInterpolation
FFX_API FfxErrorCode ffxSetFrameGenerationConfigToSwapchainVK(FfxFrameGenerationConfig const* config);

/// Query how much GPU memory is used by the FrameInterpolation swapchain.
///
/// @param [in] swapchain           The VkSwapchainKHR.
/// @param [in,out] vramUsage       The FfxApiEffectMemoryUsage output.
///
/// @retval
/// FFX_OK                              The operation completed successfully.
/// @retval
/// FFX_ERROR_INVALID_ARGUMENT          Invalid swapchain handle.
///
/// @ingroup VKFrameInterpolation
FFX_API FfxErrorCode ffxFrameInterpolationSwapchainGetGpuMemoryUsageVK(VkSwapchainKHR swapchain, FfxApiEffectMemoryUsage* vramUsage);

typedef int32_t(*FfxWaitCallbackFunc)(wchar_t* fenceName, uint64_t fenceValueToWaitFor);

typedef enum FfxFrameInterpolationSwapchainConfigureKeyVK
{
    FFX_FI_SWAPCHAIN_VK_CONFIGURE_KEY_WAITCALLBACK = 0,
    FFX_FI_SWAPCHAIN_VK_CONFIGURE_KEY_FRAMEPACINGTUNING = 2,
} FfxFrameInterpolationSwapchainConfigureKeyVK;

FFX_API FfxErrorCode ffxConfigureFrameInterpolationSwapchainVK(VkSwapchainKHR swapchain, FfxFrameInterpolationSwapchainConfigureKeyVK key, void* valuePtr);

#if defined(__cplusplus)
}
#endif // #if defined(__cplusplus)
