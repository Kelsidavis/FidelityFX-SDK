// This file is part of the Anti-Lag 2.0 SDK.
//
// Copyright (c) 2024 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
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
#include <cstring>

namespace AMD {
namespace AntiLag2VK {

    // ---------------------------------------------------------------------------
    // VK_AMD_anti_lag extension types (from vulkan_beta.h / vk_amd_anti_lag.h)
    // Defined locally to avoid requiring the latest Vulkan SDK headers.
    // ---------------------------------------------------------------------------

    #define VK_AMD_ANTI_LAG_EXTENSION_NAME "VK_AMD_anti_lag"

    typedef enum VkAntiLagModeAMD {
        VK_ANTI_LAG_MODE_DRIVER_CONTROL_AMD = 0,
        VK_ANTI_LAG_MODE_ON_AMD             = 1,
        VK_ANTI_LAG_MODE_OFF_AMD            = 2,
    } VkAntiLagModeAMD;

    typedef enum VkAntiLagStageAMD {
        VK_ANTI_LAG_STAGE_INPUT_AMD         = 0,
        VK_ANTI_LAG_STAGE_PRESENT_AMD       = 1,
    } VkAntiLagStageAMD;

    typedef struct VkAntiLagPresentationInfoAMD {
        VkStructureType    sType;
        void*              pNext;
        VkAntiLagStageAMD  stage;
        uint64_t           frameIndex;
    } VkAntiLagPresentationInfoAMD;

    typedef struct VkAntiLagDataAMD {
        VkStructureType                  sType;
        const void*                      pNext;
        VkAntiLagModeAMD                 mode;
        uint32_t                         maxFPS;
        const VkAntiLagPresentationInfoAMD* pPresentationInfo;
    } VkAntiLagDataAMD;

    // sType values (from VK_STRUCTURE_TYPE enum)
    static constexpr VkStructureType VK_STRUCTURE_TYPE_ANTI_LAG_DATA_AMD              = (VkStructureType)1000476000;
    static constexpr VkStructureType VK_STRUCTURE_TYPE_ANTI_LAG_PRESENTATION_INFO_AMD = (VkStructureType)1000476001;

    typedef void (VKAPI_PTR *PFN_vkAntiLagUpdateAMD)(VkDevice device, const VkAntiLagDataAMD* pData);

    // ---------------------------------------------------------------------------
    // Anti-Lag 2.0 Vulkan context
    // ---------------------------------------------------------------------------

    struct Context
    {
        PFN_vkAntiLagUpdateAMD  pfnAntiLagUpdate = nullptr;
        VkDevice                device           = VK_NULL_HANDLE;
        bool                    available        = false;
        bool                    enabled          = false;
        unsigned int            maxFPS           = 0;
        uint64_t                frameIndex       = 0;
    };

    // ---------------------------------------------------------------------------
    // Initialize — call once after VkDevice creation.
    // Returns true if VK_AMD_anti_lag is available on this device.
    // ---------------------------------------------------------------------------
    inline bool Initialize(Context* context, VkDevice device, VkPhysicalDevice physDevice)
    {
        if (!context || !device || !physDevice)
            return false;

        memset(context, 0, sizeof(*context));
        context->device = device;

        // Check if the extension is supported
        uint32_t extCount = 0;
        vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &extCount, nullptr);

        bool extensionFound = false;
        if (extCount > 0)
        {
            VkExtensionProperties* extensions = new VkExtensionProperties[extCount];
            vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &extCount, extensions);
            for (uint32_t i = 0; i < extCount; i++)
            {
                if (strcmp(extensions[i].extensionName, VK_AMD_ANTI_LAG_EXTENSION_NAME) == 0)
                {
                    extensionFound = true;
                    break;
                }
            }
            delete[] extensions;
        }

        if (!extensionFound)
            return false;

        // Load the function pointer
        context->pfnAntiLagUpdate = (PFN_vkAntiLagUpdateAMD)vkGetDeviceProcAddr(device, "vkAntiLagUpdateAMD");
        if (!context->pfnAntiLagUpdate)
            return false;

        context->available = true;

        // Initialize in disabled state
        VkAntiLagDataAMD data{};
        data.sType = VK_STRUCTURE_TYPE_ANTI_LAG_DATA_AMD;
        data.mode  = VK_ANTI_LAG_MODE_OFF_AMD;
        data.maxFPS = 0;
        data.pPresentationInfo = nullptr;
        context->pfnAntiLagUpdate(device, &data);

        return true;
    }

    // ---------------------------------------------------------------------------
    // DeInitialize — call before destroying the VkDevice.
    // ---------------------------------------------------------------------------
    inline void DeInitialize(Context* context)
    {
        if (context && context->available && context->pfnAntiLagUpdate)
        {
            VkAntiLagDataAMD data{};
            data.sType = VK_STRUCTURE_TYPE_ANTI_LAG_DATA_AMD;
            data.mode  = VK_ANTI_LAG_MODE_OFF_AMD;
            context->pfnAntiLagUpdate(context->device, &data);
        }
        if (context)
        {
            context->pfnAntiLagUpdate = nullptr;
            context->available = false;
            context->device = VK_NULL_HANDLE;
        }
    }

    // ---------------------------------------------------------------------------
    // Update — call once per frame before input polling.
    // Inserts the latency-reducing sleep and updates state.
    // ---------------------------------------------------------------------------
    inline bool Update(Context* context, bool enabled, unsigned int maxFPS)
    {
        if (!context || !context->available || !context->pfnAntiLagUpdate)
            return false;

        context->enabled = enabled;
        context->maxFPS  = maxFPS;

        VkAntiLagPresentationInfoAMD presentInfo{};
        presentInfo.sType      = VK_STRUCTURE_TYPE_ANTI_LAG_PRESENTATION_INFO_AMD;
        presentInfo.stage      = VK_ANTI_LAG_STAGE_INPUT_AMD;
        presentInfo.frameIndex = context->frameIndex;

        VkAntiLagDataAMD data{};
        data.sType              = VK_STRUCTURE_TYPE_ANTI_LAG_DATA_AMD;
        data.mode               = enabled ? VK_ANTI_LAG_MODE_ON_AMD : VK_ANTI_LAG_MODE_OFF_AMD;
        data.maxFPS             = maxFPS;
        data.pPresentationInfo  = &presentInfo;

        context->pfnAntiLagUpdate(context->device, &data);
        context->frameIndex++;

        return true;
    }

    // ---------------------------------------------------------------------------
    // MarkEndOfFrameRendering — call on the render thread after the main
    // rendering workload has been submitted.  Only required when frame
    // generation is enabled.
    // ---------------------------------------------------------------------------
    inline bool MarkEndOfFrameRendering(Context* context)
    {
        if (!context || !context->available || !context->pfnAntiLagUpdate)
            return false;

        // Signal the present stage to the driver
        VkAntiLagPresentationInfoAMD presentInfo{};
        presentInfo.sType      = VK_STRUCTURE_TYPE_ANTI_LAG_PRESENTATION_INFO_AMD;
        presentInfo.stage      = VK_ANTI_LAG_STAGE_PRESENT_AMD;
        presentInfo.frameIndex = context->frameIndex;

        VkAntiLagDataAMD data{};
        data.sType              = VK_STRUCTURE_TYPE_ANTI_LAG_DATA_AMD;
        data.mode               = context->enabled ? VK_ANTI_LAG_MODE_ON_AMD : VK_ANTI_LAG_MODE_OFF_AMD;
        data.maxFPS             = context->maxFPS;
        data.pPresentationInfo  = &presentInfo;

        context->pfnAntiLagUpdate(context->device, &data);
        return true;
    }

    // ---------------------------------------------------------------------------
    // SetFrameGenFrameType — call on the presentation thread just before
    // the Present call.  Tags the frame as interpolated or real.
    // ---------------------------------------------------------------------------
    inline bool SetFrameGenFrameType(Context* context, bool bInterpolatedFrame)
    {
        // VK_AMD_anti_lag handles frame type tagging implicitly via the
        // stage/frameIndex mechanism.  No separate call is needed for Vulkan.
        // This function exists for API parity with the DX12 path.
        (void)context;
        (void)bInterpolatedFrame;
        return true;
    }

} // namespace AntiLag2VK
} // namespace AMD
