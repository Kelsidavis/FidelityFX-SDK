// Stub for missing amdinternal header
#pragma once
#include <cstdint>

// Watermark functionality is not available in the open-source build.
struct FfxWatermark
{
    FfxWatermark(void*, uint32_t) {}
    void Init(void*, void*) {}
    void Apply(void*) {}
    template<typename... Args>
    void Dispatch(Args&&...) {}
};
