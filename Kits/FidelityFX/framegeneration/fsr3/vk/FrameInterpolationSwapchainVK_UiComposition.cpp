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

#include "FrameInterpolationSwapchainVK_UiComposition.h"
#include "FrameInterpolationSwapchainVK_Helpers.h"
#include "FrameInterpolationSwapchainVK.h"

void releaseUiBlitGpuResourcesVK(VkDevice device)
{
    // In the Vulkan backend, UI composition uses vkCmdBlitImage or vkCmdCopyImage
    // rather than a custom pipeline, so there are no pipeline resources to release.
    // This function exists for API symmetry with the DX12 backend.
    (void)device;
}

FFX_API ffxReturnCode_t ffxFrameInterpolationUiCompositionVK(ffxCallbackDescFrameGenerationPresent* params, void* unusedUserCtx)
{
    (void)unusedUserCtx;

    VkDevice        vkDevice   = reinterpret_cast<VkDevice>(params->device);
    VkCommandBuffer cmdBuf     = reinterpret_cast<VkCommandBuffer>(params->commandList);
    VkImage         dstImage   = reinterpret_cast<VkImage>(params->outputSwapChainBuffer.resource);
    VkImage         srcImage   = reinterpret_cast<VkImage>(params->currentBackBuffer.resource);
    VkImage         uiImage    = reinterpret_cast<VkImage>(params->currentUI.resource);

    if (!vkDevice || !cmdBuf || !dstImage || !srcImage)
        return FFX_API_RETURN_ERROR_PARAMETER;

    uint32_t width  = params->outputSwapChainBuffer.description.width;
    uint32_t height = params->outputSwapChainBuffer.description.height;

    if (!uiImage)
    {
        // No UI overlay - just copy the backbuffer to the swapchain output

        // Transition source to TRANSFER_SRC
        VkImageMemoryBarrier barriers[2] = {};

        barriers[0].sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barriers[0].oldLayout                       = ffxGetVKImageLayoutFromResourceState(params->currentBackBuffer.state);
        barriers[0].newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barriers[0].srcAccessMask                   = ffxGetVKAccessFlagsFromResourceState(params->currentBackBuffer.state);
        barriers[0].dstAccessMask                   = VK_ACCESS_TRANSFER_READ_BIT;
        barriers[0].srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barriers[0].dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barriers[0].image                           = srcImage;
        barriers[0].subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barriers[0].subresourceRange.levelCount     = 1;
        barriers[0].subresourceRange.layerCount     = 1;

        barriers[1].sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barriers[1].oldLayout                       = ffxGetVKImageLayoutFromResourceState(params->outputSwapChainBuffer.state);
        barriers[1].newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barriers[1].srcAccessMask                   = ffxGetVKAccessFlagsFromResourceState(params->outputSwapChainBuffer.state);
        barriers[1].dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
        barriers[1].srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barriers[1].dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barriers[1].image                           = dstImage;
        barriers[1].subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barriers[1].subresourceRange.levelCount     = 1;
        barriers[1].subresourceRange.layerCount     = 1;

        vkCmdPipelineBarrier(cmdBuf,
                             VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                             0, 0, nullptr, 0, nullptr, 2, barriers);

        VkImageCopy region{};
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.layerCount = 1;
        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.layerCount = 1;
        region.extent = {width, height, 1};

        vkCmdCopyImage(cmdBuf,
                       srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &region);

        // Transition back
        barriers[0].oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barriers[0].newLayout     = ffxGetVKImageLayoutFromResourceState(params->currentBackBuffer.state);
        barriers[0].srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barriers[0].dstAccessMask = ffxGetVKAccessFlagsFromResourceState(params->currentBackBuffer.state);

        barriers[1].oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barriers[1].newLayout     = ffxGetVKImageLayoutFromResourceState(params->outputSwapChainBuffer.state);
        barriers[1].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barriers[1].dstAccessMask = ffxGetVKAccessFlagsFromResourceState(params->outputSwapChainBuffer.state);

        vkCmdPipelineBarrier(cmdBuf,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                             0, 0, nullptr, 0, nullptr, 2, barriers);
    }
    else
    {
        // UI composition: blit backbuffer first, then composite UI on top.
        // For the initial implementation, we use a two-pass copy approach:
        //   1. Copy backbuffer -> output
        //   2. Blit UI -> output (using vkCmdBlitImage for potential format conversion)

        uint32_t barrierCount = 0;
        VkImageMemoryBarrier barriers[3] = {};

        // Backbuffer -> TRANSFER_SRC
        barriers[barrierCount].sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barriers[barrierCount].oldLayout                       = ffxGetVKImageLayoutFromResourceState(params->currentBackBuffer.state);
        barriers[barrierCount].newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barriers[barrierCount].srcAccessMask                   = ffxGetVKAccessFlagsFromResourceState(params->currentBackBuffer.state);
        barriers[barrierCount].dstAccessMask                   = VK_ACCESS_TRANSFER_READ_BIT;
        barriers[barrierCount].srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barriers[barrierCount].dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barriers[barrierCount].image                           = srcImage;
        barriers[barrierCount].subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barriers[barrierCount].subresourceRange.levelCount     = 1;
        barriers[barrierCount].subresourceRange.layerCount     = 1;
        barrierCount++;

        // UI -> TRANSFER_SRC
        barriers[barrierCount].sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barriers[barrierCount].oldLayout                       = ffxGetVKImageLayoutFromResourceState(params->currentUI.state);
        barriers[barrierCount].newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barriers[barrierCount].srcAccessMask                   = ffxGetVKAccessFlagsFromResourceState(params->currentUI.state);
        barriers[barrierCount].dstAccessMask                   = VK_ACCESS_TRANSFER_READ_BIT;
        barriers[barrierCount].srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barriers[barrierCount].dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barriers[barrierCount].image                           = uiImage;
        barriers[barrierCount].subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barriers[barrierCount].subresourceRange.levelCount     = 1;
        barriers[barrierCount].subresourceRange.layerCount     = 1;
        barrierCount++;

        // Output -> TRANSFER_DST
        barriers[barrierCount].sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barriers[barrierCount].oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
        barriers[barrierCount].newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barriers[barrierCount].srcAccessMask                   = 0;
        barriers[barrierCount].dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
        barriers[barrierCount].srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barriers[barrierCount].dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barriers[barrierCount].image                           = dstImage;
        barriers[barrierCount].subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barriers[barrierCount].subresourceRange.levelCount     = 1;
        barriers[barrierCount].subresourceRange.layerCount     = 1;
        barrierCount++;

        vkCmdPipelineBarrier(cmdBuf,
                             VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                             0, 0, nullptr, 0, nullptr, barrierCount, barriers);

        // Step 1: Copy backbuffer to output
        VkImageCopy copyRegion{};
        copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.srcSubresource.layerCount = 1;
        copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.dstSubresource.layerCount = 1;
        copyRegion.extent = {width, height, 1};

        vkCmdCopyImage(cmdBuf,
                       srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &copyRegion);

        // Step 2: Blit UI onto output
        // Note: This is a simple blit, not a proper alpha-blended composite.
        // A full implementation would use a compute or graphics shader for proper blending.
        // For now, we blit the UI on top which works for opaque UI overlays.
        uint32_t uiWidth  = params->currentUI.description.width;
        uint32_t uiHeight = params->currentUI.description.height;

        VkImageBlit blitRegion{};
        blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.srcSubresource.layerCount = 1;
        blitRegion.srcOffsets[1]             = {static_cast<int32_t>(uiWidth), static_cast<int32_t>(uiHeight), 1};
        blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.dstSubresource.layerCount = 1;
        blitRegion.dstOffsets[1]             = {static_cast<int32_t>(width), static_cast<int32_t>(height), 1};

        vkCmdBlitImage(cmdBuf,
                       uiImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &blitRegion, VK_FILTER_LINEAR);

        // Transition back
        for (uint32_t i = 0; i < barrierCount; ++i)
        {
            VkImageLayout tmpOld = barriers[i].oldLayout;
            barriers[i].oldLayout     = barriers[i].newLayout;
            barriers[i].newLayout     = tmpOld;
            VkAccessFlags tmpAccess   = barriers[i].srcAccessMask;
            barriers[i].srcAccessMask = barriers[i].dstAccessMask;
            barriers[i].dstAccessMask = tmpAccess;
        }

        vkCmdPipelineBarrier(cmdBuf,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                             0, 0, nullptr, 0, nullptr, barrierCount, barriers);
    }

    return FFX_API_RETURN_OK;
}
