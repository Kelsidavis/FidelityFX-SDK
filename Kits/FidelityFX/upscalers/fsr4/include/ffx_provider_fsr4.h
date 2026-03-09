#pragma once
#include "../../../api/internal/ffx_provider.h"
#include "../../../api/internal/ffx_interface.h"

// FSR4 provider ID: FSR Scale + version
#define FFX_FSR4_PROVIDER_ID ((uint64_t)0xF5A5CA1Eull << 32 | (FFX_SDK_MAKE_VERSION(4, 0, 2) & 0xFFFFFFFF))

class ffxProvider_FSR4 : public ffxProvider
{
public:
    ffxProvider_FSR4()
        : ffxProvider(FFX_FSR4_PROVIDER_ID, FFX_API_EFFECT_ID_UPSCALE, "4.0.2")
    {
    }

    virtual ~ffxProvider_FSR4() = default;

    bool IsSupported(void* device) const override;

    ffxReturnCode_t CreateContext(ffxContext* context, ffxCreateContextDescHeader* desc, Allocator& alloc) override;
    ffxReturnCode_t DestroyContext(ffxContext* context, Allocator& alloc) override;
    ffxReturnCode_t Configure(ffxContext* context, const ffxConfigureDescHeader* desc) const override;
    ffxReturnCode_t Query(ffxContext* context, ffxQueryDescHeader* desc) const override;
    ffxReturnCode_t Dispatch(ffxContext* context, const ffxDispatchDescHeader* desc) const override;

    static ffxProvider_FSR4 Instance;
};
