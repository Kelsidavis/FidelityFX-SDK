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

#include "../../api/include/vk/ffx_api_vk.h"
#include "../../api/internal/ffx_backends.h"

#include <cstring>

#if defined(FFX_FRAMEGENERATION)
#include "../../framegeneration/include/ffx_framegeneration.h"
#include "../../framegeneration/include/vk/ffx_api_framegeneration_vk.h"
#endif // defined(FFX_FRAMEGENERATION)
#if defined(FFX_UPSCALER)
#include "../../upscalers/include/ffx_upscale.h"
#endif // defined(FFX_UPSCALER)

#include "ffx_vk.h"

// ============================================================================
// CreateBackend - creates the VK FfxInterface backend
// ============================================================================
ffxReturnCode_t CreateBackend(const ffxCreateContextDescHeader *desc, bool& backendFound, FfxInterface *iface, size_t contexts, Allocator& alloc)
{
    for (const auto* it = desc->pNext; it; it = it->pNext)
    {
        switch (it->type)
        {
        case FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_VK:
        {
            if (backendFound)
                return FFX_API_RETURN_ERROR;
            backendFound = true;

            const auto *backendDesc = reinterpret_cast<const ffxCreateBackendVKDesc*>(it);
            FfxDevice device = ffxGetDeviceVK(backendDesc->vkDevice);
            size_t scratchBufferSize = ffxGetScratchMemorySizeVK(contexts);
            void* scratchBuffer = alloc.alloc(scratchBufferSize);
            memset(scratchBuffer, 0, scratchBufferSize);
            TRY2(ffxGetInterfaceVK(iface, device, backendDesc->vkPhysicalDevice, scratchBuffer, scratchBufferSize, contexts));

            break;
        }
        }
    }
    return FFX_API_RETURN_OK;
}

// ============================================================================
// GetDevice - extracts a void* device handle from descriptor chains
// ============================================================================
void* GetDevice(const ffxApiHeader* desc)
{
    for (const auto* it = desc; it; it = it->pNext)
    {
        switch (it->type)
        {
        case FFX_API_QUERY_DESC_TYPE_GET_VERSIONS:
        {
            return reinterpret_cast<const ffxQueryDescGetVersions*>(it)->device;
        }
        case FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_VK:
        {
            return reinterpret_cast<const ffxCreateBackendVKDesc*>(it)->vkDevice;
        }
#if defined(FFX_FRAMEGENERATION)
        case FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_WRAP_VK:
        {
            return reinterpret_cast<const ffxCreateContextDescFrameGenerationSwapChainWrapVK*>(it)->device;
        }
        case FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_NEW_VK:
        {
            return reinterpret_cast<const ffxCreateContextDescFrameGenerationSwapChainNewVK*>(it)->device;
        }
#endif // defined(FFX_FRAMEGENERATION)
        }
    }
    return nullptr;
}

// ============================================================================
// Provider registry - uses extern declarations to avoid #including .cpp files
// ============================================================================

// These provider GetInstance() functions are defined in their respective .cpp
// files which are compiled as separate translation units.

#if defined(FFX_FRAMEGENERATION)
// Defined in ffx_provider_fsr3framegenerationswapchain_vk.cpp
extern ffxProvider* ffxGetProviderFsr3FrameGenerationSwapChainVK();
// Defined in ffx_provider_fsr3framegeneration.cpp (backend-agnostic)
#include "../../framegeneration/fsr3/include/ffx_provider_fsr3framegeneration.h"
#endif

#if defined(FFX_UPSCALER)
#include "../../upscalers/fsr3/include/ffx_provider_fsr3upscale.h"
#include "../../upscalers/fsr3/include/ffx_provider_fsr2.h"
#endif

#if defined(FFX_FSR4)
#include "../../upscalers/fsr4/include/ffx_provider_fsr4.h"
#endif

// Build the provider list. Uses a helper function to avoid static init order issues
// with global arrays referencing singletons.
static std::span<ffxProvider* const> GetVKProviders()
{
    static ffxProvider* providers[] = {
#if defined(FFX_FSR4)
        &ffxProvider_FSR4::Instance,
#endif
#if defined(FFX_FRAMEGENERATION)
        &ffxProvider_Fsr3FrameGeneration::GetInstance(),
        ffxGetProviderFsr3FrameGenerationSwapChainVK(),
#endif
#if defined(FFX_UPSCALER)
        &ffxProvider_FSR3Upscale::GetInstance(),
        &ffxProvider_FSR2::GetInstance(),
#endif
    };
    return std::span<ffxProvider* const>(providers);
}

ffxProvider* GetProvider(ffxStructType_t descType, uint64_t overrideId, void* device, std::optional<ffxProviderExternal>& extProviderSlot)
{
    auto providers = GetVKProviders();
    return GetProvider(descType, overrideId, device, extProviderSlot, providers);
}

uint64_t GetProviderVersions(ffxStructType_t descType, void* device, uint64_t capacity, uint64_t* versionIds, const char** versionNames, std::optional<ffxProviderExternal>& extProviderSlot)
{
    auto providers = GetVKProviders();
    return GetProviderVersions(descType, device, capacity, versionIds, versionNames, extProviderSlot, providers);
}
