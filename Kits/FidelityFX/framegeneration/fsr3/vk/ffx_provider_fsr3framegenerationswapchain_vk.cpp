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

#include "../../include/vk/ffx_api_framegeneration_vk.hpp"
#include "../../../backend/vk/ffx_vk.h"
#include "../../include/ffx_framegeneration.hpp"
#include "../include/ffx_provider_fsr3framegenerationswapchain.h"
#include "../../../framegeneration/include/ffx_framegeneration.h"
#include "FrameInterpolationSwapchainVK.h"

#include <cstdlib>
#include <cstring>

#define FFX_FRAMEINTERPOLATION_SWAPCHAIN_VK_VERSION_MAJOR (FFX_FRAMEGENERATION_SWAPCHAIN_VK_VERSION_MAJOR)
#define FFX_FRAMEINTERPOLATION_SWAPCHAIN_VK_VERSION_MINOR (FFX_FRAMEGENERATION_SWAPCHAIN_VK_VERSION_MINOR)
#define FFX_FRAMEINTERPOLATION_SWAPCHAIN_VK_VERSION_PATCH (FFX_FRAMEGENERATION_SWAPCHAIN_VK_VERSION_PATCH)

/// Vulkan-specific provider for the FSR3 Frame Generation Swapchain.
class ffxProvider_Fsr3FrameGenerationSwapChainVK : public ffxProvider
{
public:
    ffxProvider_Fsr3FrameGenerationSwapChainVK();
    virtual ~ffxProvider_Fsr3FrameGenerationSwapChainVK() = default;

    virtual bool CanProvide(uint64_t type) const override;

    virtual ffxReturnCode_t CreateContext(ffxContext* context, ffxCreateContextDescHeader* desc, Allocator& alloc) override;

    virtual ffxReturnCode_t DestroyContext(ffxContext* context, Allocator& alloc) override;

    virtual ffxReturnCode_t Configure(ffxContext* context, const ffxConfigureDescHeader* desc) const override;

    virtual ffxReturnCode_t Query(ffxContext* context, ffxQueryDescHeader* desc) const override;

    virtual ffxReturnCode_t Dispatch(ffxContext* context, const ffxDispatchDescHeader* desc) const override;

    static ffxProvider_Fsr3FrameGenerationSwapChainVK& GetInstance();
};

struct InternalFgScContextVK
{
    InternalContextHeader header;
    FrameInterpolationSwapchainVK* fiSwapChain;
    VkSwapchainKHR swapchainHandle;
    uint32_t version;
};

#define STRINGIFY_(X) #X
#define STRINGIFY(X) STRINGIFY_(X)
#define MAKE_VERSION_STRING(major, minor, patch) STRINGIFY(major) "." STRINGIFY(minor) "." STRINGIFY(patch)

ffxProvider_Fsr3FrameGenerationSwapChainVK::ffxProvider_Fsr3FrameGenerationSwapChainVK()
    : ffxProvider(0xF65C'DD13i64 << 32 | (FFX_SDK_MAKE_VERSION(FFX_FRAMEINTERPOLATION_SWAPCHAIN_VK_VERSION_MAJOR, FFX_FRAMEINTERPOLATION_SWAPCHAIN_VK_VERSION_MINOR, FFX_FRAMEINTERPOLATION_SWAPCHAIN_VK_VERSION_PATCH) & 0xFFFF'FFFF),
                  FFX_API_EFFECT_ID_FRAMEGENERATIONSWAPCHAIN,
                  MAKE_VERSION_STRING(FFX_FRAMEINTERPOLATION_SWAPCHAIN_VK_VERSION_MAJOR, FFX_FRAMEINTERPOLATION_SWAPCHAIN_VK_VERSION_MINOR, FFX_FRAMEINTERPOLATION_SWAPCHAIN_VK_VERSION_PATCH))
{
}

bool ffxProvider_Fsr3FrameGenerationSwapChainVK::CanProvide(uint64_t type) const
{
    return ((type & FFX_API_EFFECT_MASK) == FFX_API_EFFECT_ID_FRAMEGENERATIONSWAPCHAIN &&
            (type & FFX_API_BACKEND_MASK) == FFX_API_BACKEND_ID_VK);
}

ffxReturnCode_t ffxProvider_Fsr3FrameGenerationSwapChainVK::CreateContext(ffxContext* context, ffxCreateContextDescHeader* header, Allocator& alloc)
{
    if (auto desc = ffx::DynamicCast<ffxCreateContextDescFrameGenerationSwapChainWrapVK>(header))
    {
        InternalFgScContextVK* internal_context = alloc.construct<InternalFgScContextVK>();
        VERIFY(internal_context, FFX_API_RETURN_ERROR_MEMORY);
        internal_context->header.provider = this;

        // Parse version
        uint32_t version = FFX_FRAMEGENERATION_SWAPCHAIN_VK_VERSION;
        for (auto it = header; it; it = it->pNext)
        {
            if (auto versionDesc = ffx::DynamicCast<ffxCreateContextDescFrameGenerationSwapChainVersionVK>(it))
            {
                version = versionDesc->version;
                break;
            }
        }
        internal_context->version = version;

        auto* proxy = new FrameInterpolationSwapchainVK();
        FfxErrorCode result = proxy->init(desc->device, desc->physicalDevice,
                                           desc->gameQueue, desc->gameQueueFamilyIndex,
                                           *desc->swapchain, desc->surface);
        if (result != FFX_OK)
        {
            delete proxy;
            alloc.dealloc(internal_context);
            return FFX_API_RETURN_ERROR_RUNTIME_ERROR;
        }

        ffxRegisterFrameInterpolationSwapchainVK(*desc->swapchain, proxy);

        internal_context->fiSwapChain = proxy;
        internal_context->swapchainHandle = *desc->swapchain;

        *context = internal_context;
        return FFX_API_RETURN_OK;
    }
    else if (auto desc = ffx::DynamicCast<ffxCreateContextDescFrameGenerationSwapChainNewVK>(header))
    {
        InternalFgScContextVK* internal_context = alloc.construct<InternalFgScContextVK>();
        VERIFY(internal_context, FFX_API_RETURN_ERROR_MEMORY);
        internal_context->header.provider = this;

        uint32_t version = FFX_FRAMEGENERATION_SWAPCHAIN_VK_VERSION;
        for (auto it = header; it; it = it->pNext)
        {
            if (auto versionDesc = ffx::DynamicCast<ffxCreateContextDescFrameGenerationSwapChainVersionVK>(it))
            {
                version = versionDesc->version;
                break;
            }
        }
        internal_context->version = version;

        auto* proxy = new FrameInterpolationSwapchainVK();
        FfxErrorCode result = proxy->initNew(desc->device, desc->physicalDevice,
                                              desc->gameQueue, desc->gameQueueFamilyIndex,
                                              desc->createInfo);
        if (result != FFX_OK)
        {
            delete proxy;
            alloc.dealloc(internal_context);
            return FFX_API_RETURN_ERROR_RUNTIME_ERROR;
        }

        VkSwapchainKHR handle = proxy->getRealSwapchain();
        *desc->swapchain = handle;

        ffxRegisterFrameInterpolationSwapchainVK(handle, proxy);

        internal_context->fiSwapChain = proxy;
        internal_context->swapchainHandle = handle;

        *context = internal_context;
        return FFX_API_RETURN_OK;
    }
    else
    {
        return FFX_API_RETURN_ERROR_UNKNOWN_DESCTYPE;
    }
}

ffxReturnCode_t ffxProvider_Fsr3FrameGenerationSwapChainVK::DestroyContext(ffxContext* context, Allocator& alloc)
{
    VERIFY(context, FFX_API_RETURN_ERROR_PARAMETER);
    VERIFY(*context, FFX_API_RETURN_ERROR_PARAMETER);

    InternalFgScContextVK* internal_context = reinterpret_cast<InternalFgScContextVK*>(*context);

    ffxUnregisterFrameInterpolationSwapchainVK(internal_context->swapchainHandle);

    if (internal_context->fiSwapChain)
    {
        internal_context->fiSwapChain->shutdown();
        delete internal_context->fiSwapChain;
        internal_context->fiSwapChain = nullptr;
    }

    alloc.dealloc(internal_context);

    return FFX_API_RETURN_OK;
}

ffxReturnCode_t ffxProvider_Fsr3FrameGenerationSwapChainVK::Configure(ffxContext* context, const ffxConfigureDescHeader* header) const
{
    VERIFY(header, FFX_API_RETURN_ERROR_PARAMETER);
    VERIFY(context, FFX_API_RETURN_ERROR_PARAMETER);
    VERIFY(*context, FFX_API_RETURN_ERROR_PARAMETER);

    InternalFgScContextVK* internal_context = reinterpret_cast<InternalFgScContextVK*>(*context);

    if (auto desc = ffx::DynamicCast<ffxConfigureDescFrameGenerationSwapChainRegisterUiResourceVK>(header))
    {
        internal_context->fiSwapChain->registerUiResource(desc->uiResource, desc->flags);
        return FFX_API_RETURN_OK;
    }
    else if (auto desc = ffx::DynamicCast<ffxConfigureDescFrameGenerationSwapChainKeyValueVK>(header))
    {
        switch (desc->key)
        {
        case FFX_API_CONFIGURE_FG_SWAPCHAIN_KEY_VK_WAITCALLBACK:
            internal_context->fiSwapChain->setWaitCallback(reinterpret_cast<FfxWaitCallbackFunc>(desc->ptr));
            break;
        case FFX_API_CONFIGURE_FG_SWAPCHAIN_KEY_VK_FRAMEPACINGTUNING:
            internal_context->fiSwapChain->setFramePacingTuning(reinterpret_cast<const FfxApiSwapchainFramePacingTuningVK*>(desc->ptr));
            break;
        default:
            return FFX_API_RETURN_ERROR_PARAMETER;
        }
        return FFX_API_RETURN_OK;
    }
    else
    {
        return FFX_API_RETURN_ERROR_PARAMETER;
    }
}

ffxReturnCode_t ffxProvider_Fsr3FrameGenerationSwapChainVK::Query(ffxContext* context, ffxQueryDescHeader* header) const
{
    VERIFY(header, FFX_API_RETURN_ERROR_PARAMETER);

    if (auto desc = ffx::DynamicCast<ffxQueryDescFrameGenerationSwapChainInterpolationCommandListVK>(header))
    {
        VERIFY(context, FFX_API_RETURN_ERROR_PARAMETER);
        VERIFY(*context, FFX_API_RETURN_ERROR_PARAMETER);

        InternalFgScContextVK* internal_context = reinterpret_cast<InternalFgScContextVK*>(*context);
        VkCommandBuffer cmdBuf = internal_context->fiSwapChain->getInterpolationCommandBuffer();
        *desc->pOutCommandList = reinterpret_cast<void*>(cmdBuf);

        return FFX_API_RETURN_OK;
    }
    else if (auto desc = ffx::DynamicCast<ffxQueryDescFrameGenerationSwapChainInterpolationTextureVK>(header))
    {
        VERIFY(context, FFX_API_RETURN_ERROR_PARAMETER);
        VERIFY(*context, FFX_API_RETURN_ERROR_PARAMETER);

        InternalFgScContextVK* internal_context = reinterpret_cast<InternalFgScContextVK*>(*context);
        *desc->pOutTexture = internal_context->fiSwapChain->interpolationOutput();

        return FFX_API_RETURN_OK;
    }
    else if (auto desc = ffx::DynamicCast<ffxQueryFrameGenerationSwapChainGetGPUMemoryUsageVK>(header))
    {
        VERIFY(context, FFX_API_RETURN_ERROR_PARAMETER);
        VERIFY(*context, FFX_API_RETURN_ERROR_PARAMETER);
        VERIFY(desc->gpuMemoryUsageFrameGenerationSwapchain, FFX_API_RETURN_ERROR_PARAMETER);

        memset(desc->gpuMemoryUsageFrameGenerationSwapchain, 0, sizeof(FfxApiEffectMemoryUsage));

        InternalFgScContextVK* internal_context = reinterpret_cast<InternalFgScContextVK*>(*context);
        internal_context->fiSwapChain->getGpuMemoryUsage(desc->gpuMemoryUsageFrameGenerationSwapchain);
        return FFX_API_RETURN_OK;
    }
    else
    {
        return FFX_API_RETURN_ERROR_UNKNOWN_DESCTYPE;
    }
}

ffxReturnCode_t ffxProvider_Fsr3FrameGenerationSwapChainVK::Dispatch(ffxContext* context, const ffxDispatchDescHeader* header) const
{
    VERIFY(*context, FFX_API_RETURN_ERROR_PARAMETER);

    InternalFgScContextVK* internal_context = reinterpret_cast<InternalFgScContextVK*>(*context);

    if (auto desc = ffx::DynamicCast<ffxDispatchDescFrameGenerationSwapChainWaitForPresentsVK>(header))
    {
        internal_context->fiSwapChain->waitForPresents();
        return FFX_API_RETURN_OK;
    }
    else
    {
        return FFX_API_RETURN_ERROR;
    }
}

ffxProvider_Fsr3FrameGenerationSwapChainVK& ffxProvider_Fsr3FrameGenerationSwapChainVK::GetInstance()
{
    static ffxProvider_Fsr3FrameGenerationSwapChainVK instance;
    return instance;
}
