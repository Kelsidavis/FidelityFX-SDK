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
#include <cstdint>
#include <cstring>
#include <cmath>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "../../../api/internal/ffx_assert.h"

typedef int32_t FfxErrorCode;
typedef int32_t(*FfxWaitCallbackFunc)(wchar_t* fenceName, uint64_t fenceValueToWaitFor);

/// A structure for tuning the frame pacing behavior.
typedef struct FfxApiSwapchainFramePacingTuningVK
{
    double   safetyMarginInSec;
    double   varianceFactor;
    bool     allowHybridSpin;
    uint32_t hybridSpinTime;
    bool     allowWaitForSingleObjectOnFence;
} FfxApiSwapchainFramePacingTuningVK;

void waitForTimelineValue(VkDevice device, VkSemaphore semaphore, uint64_t value, uint64_t timeout = UINT64_MAX);
bool waitForTimelineValueNonBlocking(VkDevice device, VkSemaphore semaphore, uint64_t value);
void waitForPerformanceCount(int64_t targetNanoseconds);
uint64_t getTimeNanoseconds();

/// Simple moving average for frame pacing
template <const int Size, typename Type = double>
struct SimpleMovingAverage
{
    Type                    history[Size] = {};
    unsigned int            idx           = 0;
    unsigned int            updateCount   = 0;

    Type getAverage()
    {
        if (updateCount < Size)
            return 0.0;

        Type          average    = 0.f;
        unsigned int  iterations = (updateCount >= Size) ? Size : updateCount;

        if (iterations > 0)
        {
            for (size_t i = 0; i < iterations; i++)
            {
                average += history[i];
            }
            average /= iterations;
        }

        return average;
    }

    Type getVariance()
    {
        if (updateCount < Size)
            return 0.0;

        Type average  = getAverage();
        Type variance = 0.f;
        unsigned int iterations = (updateCount >= Size) ? Size : updateCount;

        if (iterations > 0)
        {
            for (size_t i = 0; i < iterations; i++)
            {
                variance += (history[i] - average) * (history[i] - average);
            }
            variance /= iterations;
        }

        return sqrt(variance);
    }

    void reset()
    {
        updateCount = 0;
        idx         = 0;
    }

    void update(Type newValue)
    {
        history[idx] = newValue;
        idx          = (idx + 1) % Size;
        updateCount++;
    }
};

/// Vulkan command buffer pool for managing per-queue command allocators.
class VkCommandPool_
{
    VkDevice                device       = VK_NULL_HANDLE;
    VkCommandPool           pool         = VK_NULL_HANDLE;
    VkCommandBuffer*        buffers      = nullptr;
    VkFence*                fences       = nullptr;
    uint32_t                capacity     = 0;
    uint32_t                nextIndex    = 0;
    std::mutex              mutex;

public:
    VkCommandPool_() = default;
    ~VkCommandPool_() { destroy(); }

    bool init(VkDevice dev, uint32_t queueFamilyIndex, uint32_t count);
    void destroy();

    /// Returns an available command buffer, waiting on its fence if needed.
    VkCommandBuffer acquire(VkFence* outFence = nullptr);

    /// Submits the command buffer to the given queue with optional semaphore synchronization.
    VkResult submit(VkCommandBuffer cmdBuf, VkQueue queue,
                    VkSemaphore waitSemaphore = VK_NULL_HANDLE,
                    VkPipelineStageFlags waitStage = 0,
                    VkSemaphore signalSemaphore = VK_NULL_HANDLE,
                    uint64_t signalTimelineValue = 0);
};

/// Convert FfxApiResourceState to VkImageLayout
VkImageLayout ffxGetVKImageLayoutFromResourceState(uint32_t state);

/// Convert FfxApiResourceState to VkAccessFlags
VkAccessFlags ffxGetVKAccessFlagsFromResourceState(uint32_t state);

/// Convert FfxApiSurfaceFormat to VkFormat for creating image views
VkFormat ffxConvertFormatSrvVK(VkFormat format);

/// Get estimated GPU memory size for a VkImage (approximate)
uint64_t getResourceGpuMemorySizeVK(VkDevice device, VkImage image);
