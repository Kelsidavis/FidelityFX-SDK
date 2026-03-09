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
#include "../../../api/internal/ffx_internal_types.h"
#include "../../../api/internal/ffx_error.h"

/// Initialize the debug pacing overlay resources (pipeline, render pass).
/// Call once after device creation.  Thread-safe; lazy-initializes on first use.
FfxErrorCode ffxDebugPacingInitVK(VkDevice device, VkFormat swapchainFormat);

/// Render the debug pacing overlay (two colored bars in the top-left corner)
/// into the given command buffer targeting the specified image.
/// The image must be in COLOR_ATTACHMENT_OPTIMAL layout on entry and will be
/// left in that layout on exit.
FfxErrorCode ffxDebugPacingRenderVK(VkDevice device, VkCommandBuffer cmdBuf,
                                    VkImage targetImage, VkImageView targetView,
                                    VkFormat format, uint32_t width, uint32_t height,
                                    uint32_t frameIndex);

/// Release all GPU resources used by the debug pacing overlay.
void ffxDebugPacingReleaseVK(VkDevice device);
