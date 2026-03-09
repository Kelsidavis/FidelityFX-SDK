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

#include "FrameInterpolationSwapchainVK_Helpers.h"
#include <thread>
#include <algorithm>

void waitForTimelineValue(VkDevice device, VkSemaphore semaphore, uint64_t value, uint64_t timeout)
{
    VkSemaphoreWaitInfo waitInfo{};
    waitInfo.sType          = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores    = &semaphore;
    waitInfo.pValues        = &value;
    vkWaitSemaphores(device, &waitInfo, timeout);
}

bool waitForTimelineValueNonBlocking(VkDevice device, VkSemaphore semaphore, uint64_t value)
{
    VkSemaphoreWaitInfo waitInfo{};
    waitInfo.sType          = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores    = &semaphore;
    waitInfo.pValues        = &value;
    return vkWaitSemaphores(device, &waitInfo, 0) == VK_SUCCESS;
}

uint64_t getTimeNanoseconds()
{
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count());
}

void waitForPerformanceCount(int64_t targetNanoseconds)
{
    int64_t now = static_cast<int64_t>(getTimeNanoseconds());
    int64_t remaining = targetNanoseconds - now;

    if (remaining <= 0)
        return;

    // Sleep for the bulk of the wait, then spin for precision
    if (remaining > 2000000) // >2ms
    {
        std::this_thread::sleep_for(std::chrono::nanoseconds(remaining - 1000000));
    }

    // Spin-wait for the remaining time
    while (static_cast<int64_t>(getTimeNanoseconds()) < targetNanoseconds)
    {
        std::this_thread::yield();
    }
}

// VkCommandPool_ implementation

bool VkCommandPool_::init(VkDevice dev, uint32_t queueFamilyIndex, uint32_t count)
{
    device = dev;
    capacity = count;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndex;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &pool) != VK_SUCCESS)
        return false;

    buffers = new VkCommandBuffer[count];
    fences  = new VkFence[count];

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = pool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    if (vkAllocateCommandBuffers(device, &allocInfo, buffers) != VK_SUCCESS)
    {
        delete[] buffers;
        delete[] fences;
        buffers = nullptr;
        fences = nullptr;
        return false;
    }

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < count; ++i)
    {
        if (vkCreateFence(device, &fenceInfo, nullptr, &fences[i]) != VK_SUCCESS)
        {
            for (uint32_t j = 0; j < i; ++j)
                vkDestroyFence(device, fences[j], nullptr);
            delete[] buffers;
            delete[] fences;
            buffers = nullptr;
            fences = nullptr;
            return false;
        }
    }

    return true;
}

void VkCommandPool_::destroy()
{
    if (device == VK_NULL_HANDLE)
        return;

    if (fences)
    {
        for (uint32_t i = 0; i < capacity; ++i)
        {
            vkWaitForFences(device, 1, &fences[i], VK_TRUE, UINT64_MAX);
            vkDestroyFence(device, fences[i], nullptr);
        }
        delete[] fences;
        fences = nullptr;
    }

    if (buffers)
    {
        vkFreeCommandBuffers(device, pool, capacity, buffers);
        delete[] buffers;
        buffers = nullptr;
    }

    if (pool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(device, pool, nullptr);
        pool = VK_NULL_HANDLE;
    }

    device = VK_NULL_HANDLE;
}

VkCommandBuffer VkCommandPool_::acquire(VkFence* outFence)
{
    std::lock_guard<std::mutex> lock(mutex);

    uint32_t index = nextIndex;
    nextIndex = (nextIndex + 1) % capacity;

    // Wait for this command buffer's fence to be signaled
    vkWaitForFences(device, 1, &fences[index], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &fences[index]);

    vkResetCommandBuffer(buffers[index], 0);

    if (outFence)
        *outFence = fences[index];

    return buffers[index];
}

VkResult VkCommandPool_::submit(VkCommandBuffer cmdBuf, VkQueue queue,
                                VkSemaphore waitSemaphore, VkPipelineStageFlags waitStage,
                                VkSemaphore signalSemaphore, uint64_t signalTimelineValue)
{
    // Find the fence associated with this command buffer
    VkFence fence = VK_NULL_HANDLE;
    for (uint32_t i = 0; i < capacity; ++i)
    {
        if (buffers[i] == cmdBuf)
        {
            fence = fences[i];
            break;
        }
    }

    VkTimelineSemaphoreSubmitInfo timelineInfo{};
    timelineInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;

    uint64_t waitValue = 0; // binary semaphore wait
    uint64_t signalValue = signalTimelineValue;

    VkSubmitInfo submitInfo{};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &cmdBuf;

    if (waitSemaphore != VK_NULL_HANDLE)
    {
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores    = &waitSemaphore;
        submitInfo.pWaitDstStageMask  = &waitStage;
    }

    if (signalSemaphore != VK_NULL_HANDLE)
    {
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = &signalSemaphore;

        if (signalTimelineValue > 0)
        {
            timelineInfo.signalSemaphoreValueCount = 1;
            timelineInfo.pSignalSemaphoreValues    = &signalValue;
            if (waitSemaphore != VK_NULL_HANDLE)
            {
                timelineInfo.waitSemaphoreValueCount = 1;
                timelineInfo.pWaitSemaphoreValues    = &waitValue;
            }
            submitInfo.pNext = &timelineInfo;
        }
    }

    return vkQueueSubmit(queue, 1, &submitInfo, fence);
}

VkImageLayout ffxGetVKImageLayoutFromResourceState(uint32_t state)
{
    if (state & FFX_API_RESOURCE_STATE_UNORDERED_ACCESS)
        return VK_IMAGE_LAYOUT_GENERAL;
    if (state & FFX_API_RESOURCE_STATE_COMPUTE_READ)
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    if (state & FFX_API_RESOURCE_STATE_PIXEL_READ)
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    if (state & FFX_API_RESOURCE_STATE_COPY_SRC)
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    if (state & FFX_API_RESOURCE_STATE_COPY_DEST)
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    if (state & FFX_API_RESOURCE_STATE_PRESENT)
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    if (state & FFX_API_RESOURCE_STATE_RENDER_TARGET)
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    if (state & FFX_API_RESOURCE_STATE_DEPTH_ATTACHMENT)
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    return VK_IMAGE_LAYOUT_GENERAL;
}

VkAccessFlags ffxGetVKAccessFlagsFromResourceState(uint32_t state)
{
    VkAccessFlags flags = 0;
    if (state & FFX_API_RESOURCE_STATE_UNORDERED_ACCESS)
        flags |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    if (state & FFX_API_RESOURCE_STATE_COMPUTE_READ)
        flags |= VK_ACCESS_SHADER_READ_BIT;
    if (state & FFX_API_RESOURCE_STATE_PIXEL_READ)
        flags |= VK_ACCESS_SHADER_READ_BIT;
    if (state & FFX_API_RESOURCE_STATE_COPY_SRC)
        flags |= VK_ACCESS_TRANSFER_READ_BIT;
    if (state & FFX_API_RESOURCE_STATE_COPY_DEST)
        flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
    if (state & FFX_API_RESOURCE_STATE_RENDER_TARGET)
        flags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    if (state & FFX_API_RESOURCE_STATE_DEPTH_ATTACHMENT)
        flags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    return flags;
}

VkFormat ffxConvertFormatSrvVK(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_D32_SFLOAT:
        return VK_FORMAT_R32_SFLOAT;
    case VK_FORMAT_D16_UNORM:
        return VK_FORMAT_R16_UNORM;
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return VK_FORMAT_R32_SFLOAT;
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return VK_FORMAT_R32_UINT;
    default:
        return format;
    }
}

uint64_t getResourceGpuMemorySizeVK(VkDevice device, VkImage image)
{
    VkMemoryRequirements memReqs{};
    vkGetImageMemoryRequirements(device, image, &memReqs);
    return memReqs.size;
}
