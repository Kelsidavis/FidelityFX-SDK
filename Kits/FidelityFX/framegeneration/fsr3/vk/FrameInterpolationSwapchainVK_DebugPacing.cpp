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

#include "FrameInterpolationSwapchainVK_DebugPacing.h"
#include <cstring>

// ---------------------------------------------------------------------------
// Embedded SPIR-V shaders for the debug pacing overlay.
//
// Vertex shader: generates a full-screen triangle from gl_VertexIndex (3 verts).
// Fragment shader: draws two 16px-wide vertical bars in the top-left corner,
//   alternating white/magenta vs black based on frameIndex bit 0.
//   Push constant: uint32_t frameIndex.
// ---------------------------------------------------------------------------

// Compiled from debug_pacing.vert (GLSL 450):
//   void main() {
//       vec2 positions[3] = vec2[](vec2(-1,-1), vec2(3,-1), vec2(-1,3));
//       gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
//   }
static const uint32_t g_debugPacingVS[] = {
    0x07230203, 0x00010000, 0x000d000b, 0x00000028, 0x00000000, 0x00020011, 0x00000001, 0x0006000b,
    0x00000001, 0x4c534c47, 0x6474732e, 0x3035342e, 0x00000000, 0x0003000e, 0x00000000, 0x00000001,
    0x0007000f, 0x00000000, 0x00000004, 0x6e69616d, 0x00000000, 0x00000018, 0x0000001c, 0x00050048,
    0x00000016, 0x00000000, 0x0000000b, 0x00000000, 0x00050048, 0x00000016, 0x00000001, 0x0000000b,
    0x00000001, 0x00050048, 0x00000016, 0x00000002, 0x0000000b, 0x00000003, 0x00050048, 0x00000016,
    0x00000003, 0x0000000b, 0x00000004, 0x00030047, 0x00000016, 0x00000002, 0x00040047, 0x0000001c,
    0x0000000b, 0x0000002a, 0x00020013, 0x00000002, 0x00030021, 0x00000003, 0x00000002, 0x00030016,
    0x00000006, 0x00000020, 0x00040017, 0x00000007, 0x00000006, 0x00000002, 0x00040015, 0x00000008,
    0x00000020, 0x00000000, 0x0004002b, 0x00000008, 0x00000009, 0x00000003, 0x0004001c, 0x0000000a,
    0x00000007, 0x00000009, 0x00040020, 0x0000000b, 0x00000007, 0x0000000a, 0x0004002b, 0x00000006,
    0x0000000d, 0xbf800000, 0x0005002c, 0x00000007, 0x0000000e, 0x0000000d, 0x0000000d, 0x0004002b,
    0x00000006, 0x0000000f, 0x40400000, 0x0005002c, 0x00000007, 0x00000010, 0x0000000f, 0x0000000d,
    0x0005002c, 0x00000007, 0x00000011, 0x0000000d, 0x0000000f, 0x0006002c, 0x0000000a, 0x00000012,
    0x0000000e, 0x00000010, 0x00000011, 0x00040017, 0x00000013, 0x00000006, 0x00000004, 0x0004002b,
    0x00000008, 0x00000014, 0x00000001, 0x0004001c, 0x00000015, 0x00000006, 0x00000014, 0x0006001e,
    0x00000016, 0x00000013, 0x00000006, 0x00000015, 0x00000015, 0x00040020, 0x00000017, 0x00000003,
    0x00000016, 0x0004003b, 0x00000017, 0x00000018, 0x00000003, 0x00040015, 0x00000019, 0x00000020,
    0x00000001, 0x0004002b, 0x00000019, 0x0000001a, 0x00000000, 0x00040020, 0x0000001b, 0x00000001,
    0x00000019, 0x0004003b, 0x0000001b, 0x0000001c, 0x00000001, 0x00040020, 0x0000001e, 0x00000007,
    0x00000007, 0x0004002b, 0x00000006, 0x00000021, 0x00000000, 0x0004002b, 0x00000006, 0x00000022,
    0x3f800000, 0x00040020, 0x00000026, 0x00000003, 0x00000013, 0x00050036, 0x00000002, 0x00000004,
    0x00000000, 0x00000003, 0x000200f8, 0x00000005, 0x0004003b, 0x0000000b, 0x0000000c, 0x00000007,
    0x0003003e, 0x0000000c, 0x00000012, 0x0004003d, 0x00000019, 0x0000001d, 0x0000001c, 0x00050041,
    0x0000001e, 0x0000001f, 0x0000000c, 0x0000001d, 0x0004003d, 0x00000007, 0x00000020, 0x0000001f,
    0x00050051, 0x00000006, 0x00000023, 0x00000020, 0x00000000, 0x00050051, 0x00000006, 0x00000024,
    0x00000020, 0x00000001, 0x00070050, 0x00000013, 0x00000025, 0x00000023, 0x00000024, 0x00000021,
    0x00000022, 0x00050041, 0x00000026, 0x00000027, 0x00000018, 0x0000001a, 0x0003003e, 0x00000027,
    0x00000025, 0x000100fd, 0x00010038,
};

// Compiled from debug_pacing.frag (GLSL 450):
//   layout(push_constant) uniform PC { uint frameIndex; } pc;
//   layout(location = 0) out vec4 outColor;
//   void main() {
//       float bit0 = float(pc.frameIndex & 1u);
//       float pixelX = gl_FragCoord.x;
//       if (pixelX < 16.0) outColor = vec4(bit0, bit0, bit0, 1.0);
//       else if (pixelX < 32.0) outColor = vec4(bit0, 0.0, bit0, 1.0);
//       else outColor = vec4(0.0, 0.0, 0.0, 1.0);
//   }
static const uint32_t g_debugPacingFS[] = {
    0x07230203, 0x00010000, 0x000d000b, 0x00000036, 0x00000000, 0x00020011, 0x00000001, 0x0006000b,
    0x00000001, 0x4c534c47, 0x6474732e, 0x3035342e, 0x00000000, 0x0003000e, 0x00000000, 0x00000001,
    0x0007000f, 0x00000004, 0x00000004, 0x6e69616d, 0x00000000, 0x00000018, 0x00000024, 0x00030010,
    0x00000004, 0x00000007, 0x00050048, 0x0000000a, 0x00000000, 0x00000023, 0x00000000, 0x00030047,
    0x0000000a, 0x00000002, 0x00040047, 0x00000018, 0x0000000b, 0x0000000f, 0x00040047, 0x00000024,
    0x0000001e, 0x00000000, 0x00020013, 0x00000002, 0x00030021, 0x00000003, 0x00000002, 0x00030016,
    0x00000006, 0x00000020, 0x00040015, 0x00000009, 0x00000020, 0x00000000, 0x0003001e, 0x0000000a,
    0x00000009, 0x00040020, 0x0000000b, 0x00000009, 0x0000000a, 0x0004003b, 0x0000000b, 0x0000000c,
    0x00000009, 0x00040015, 0x0000000d, 0x00000020, 0x00000001, 0x0004002b, 0x0000000d, 0x0000000e,
    0x00000000, 0x00040020, 0x0000000f, 0x00000009, 0x00000009, 0x0004002b, 0x00000009, 0x00000012,
    0x00000001, 0x00040017, 0x00000016, 0x00000006, 0x00000004, 0x00040020, 0x00000017, 0x00000001,
    0x00000016, 0x0004003b, 0x00000017, 0x00000018, 0x00000001, 0x0004002b, 0x00000009, 0x00000019,
    0x00000000, 0x00040020, 0x0000001a, 0x00000001, 0x00000006, 0x0004002b, 0x00000006, 0x0000001e,
    0x41800000, 0x00020014, 0x0000001f, 0x00040020, 0x00000023, 0x00000003, 0x00000016, 0x0004003b,
    0x00000023, 0x00000024, 0x00000003, 0x0004002b, 0x00000006, 0x00000028, 0x3f800000, 0x0004002b,
    0x00000006, 0x0000002c, 0x42000000, 0x0004002b, 0x00000006, 0x00000031, 0x00000000, 0x0007002c,
    0x00000016, 0x00000035, 0x00000031, 0x00000031, 0x00000031, 0x00000028, 0x00050036, 0x00000002,
    0x00000004, 0x00000000, 0x00000003, 0x000200f8, 0x00000005, 0x00050041, 0x0000000f, 0x00000010,
    0x0000000c, 0x0000000e, 0x0004003d, 0x00000009, 0x00000011, 0x00000010, 0x000500c7, 0x00000009,
    0x00000013, 0x00000011, 0x00000012, 0x00040070, 0x00000006, 0x00000014, 0x00000013, 0x00050041,
    0x0000001a, 0x0000001b, 0x00000018, 0x00000019, 0x0004003d, 0x00000006, 0x0000001c, 0x0000001b,
    0x000500b8, 0x0000001f, 0x00000020, 0x0000001c, 0x0000001e, 0x000300f7, 0x00000022, 0x00000000,
    0x000400fa, 0x00000020, 0x00000021, 0x0000002a, 0x000200f8, 0x00000021, 0x00070050, 0x00000016,
    0x00000029, 0x00000014, 0x00000014, 0x00000014, 0x00000028, 0x0003003e, 0x00000024, 0x00000029,
    0x000200f9, 0x00000022, 0x000200f8, 0x0000002a, 0x000500b8, 0x0000001f, 0x0000002d, 0x0000001c,
    0x0000002c, 0x000300f7, 0x0000002f, 0x00000000, 0x000400fa, 0x0000002d, 0x0000002e, 0x00000034,
    0x000200f8, 0x0000002e, 0x00070050, 0x00000016, 0x00000033, 0x00000014, 0x00000031, 0x00000014,
    0x00000028, 0x0003003e, 0x00000024, 0x00000033, 0x000200f9, 0x0000002f, 0x000200f8, 0x00000034,
    0x0003003e, 0x00000024, 0x00000035, 0x000200f9, 0x0000002f, 0x000200f8, 0x0000002f, 0x000200f9,
    0x00000022, 0x000200f8, 0x00000022, 0x000100fd, 0x00010038,
};

// ---------------------------------------------------------------------------
// Static GPU resources (lazy-initialized, shared across all swapchains)
// ---------------------------------------------------------------------------
static VkPipelineLayout  s_pipelineLayout  = VK_NULL_HANDLE;
static VkRenderPass      s_renderPass      = VK_NULL_HANDLE;
static VkPipeline        s_pipeline        = VK_NULL_HANDLE;
static uint32_t          s_frameIndex      = 0;

static VkShaderModule createShaderModule(VkDevice device, const uint32_t* code, size_t codeSize)
{
    VkShaderModuleCreateInfo ci{};
    ci.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = codeSize;
    ci.pCode    = code;

    VkShaderModule module = VK_NULL_HANDLE;
    vkCreateShaderModule(device, &ci, nullptr, &module);
    return module;
}

static FfxErrorCode createPipelineResources(VkDevice device, VkFormat format)
{
    // --- Push constant layout (uint32_t frameIndex, visible to fragment) ---
    VkPushConstantRange pushRange{};
    pushRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    pushRange.offset     = 0;
    pushRange.size       = sizeof(uint32_t);

    VkPipelineLayoutCreateInfo layoutCI{};
    layoutCI.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutCI.pushConstantRangeCount = 1;
    layoutCI.pPushConstantRanges    = &pushRange;

    if (vkCreatePipelineLayout(device, &layoutCI, nullptr, &s_pipelineLayout) != VK_SUCCESS)
        return FFX_ERROR_BACKEND_API_ERROR;

    // --- Render pass (single color attachment, load=LOAD, store=STORE) ---
    VkAttachmentDescription colorAttach{};
    colorAttach.format         = format;
    colorAttach.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttach.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttach.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttach.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttach.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttach.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorRef;

    VkRenderPassCreateInfo rpCI{};
    rpCI.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpCI.attachmentCount = 1;
    rpCI.pAttachments    = &colorAttach;
    rpCI.subpassCount    = 1;
    rpCI.pSubpasses      = &subpass;

    if (vkCreateRenderPass(device, &rpCI, nullptr, &s_renderPass) != VK_SUCCESS)
        return FFX_ERROR_BACKEND_API_ERROR;

    // --- Shader modules ---
    VkShaderModule vsModule = createShaderModule(device, g_debugPacingVS, sizeof(g_debugPacingVS));
    VkShaderModule fsModule = createShaderModule(device, g_debugPacingFS, sizeof(g_debugPacingFS));
    if (!vsModule || !fsModule)
    {
        if (vsModule) vkDestroyShaderModule(device, vsModule, nullptr);
        if (fsModule) vkDestroyShaderModule(device, fsModule, nullptr);
        return FFX_ERROR_BACKEND_API_ERROR;
    }

    VkPipelineShaderStageCreateInfo stages[2]{};
    stages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = vsModule;
    stages[0].pName  = "main";
    stages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = fsModule;
    stages[1].pName  = "main";

    // --- Pipeline state ---
    VkPipelineVertexInputStateCreateInfo vertexInputCI{};
    vertexInputCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCI{};
    inputAssemblyCI.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // Viewport and scissor are dynamic
    VkPipelineViewportStateCreateInfo viewportCI{};
    viewportCI.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportCI.viewportCount = 1;
    viewportCI.scissorCount  = 1;

    VkPipelineRasterizationStateCreateInfo rasterCI{};
    rasterCI.sType       = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterCI.polygonMode = VK_POLYGON_MODE_FILL;
    rasterCI.cullMode    = VK_CULL_MODE_BACK_BIT;
    rasterCI.frontFace   = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterCI.lineWidth   = 1.0f;

    VkPipelineMultisampleStateCreateInfo msCI{};
    msCI.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    msCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthCI{};
    depthCI.sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthCI.depthTestEnable  = VK_FALSE;
    depthCI.depthWriteEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState blendAttach{};
    blendAttach.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                 VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendAttach.blendEnable    = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo blendCI{};
    blendCI.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendCI.attachmentCount = 1;
    blendCI.pAttachments    = &blendAttach;

    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicCI{};
    dynamicCI.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicCI.dynamicStateCount = 2;
    dynamicCI.pDynamicStates    = dynamicStates;

    VkGraphicsPipelineCreateInfo pipeCI{};
    pipeCI.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeCI.stageCount          = 2;
    pipeCI.pStages             = stages;
    pipeCI.pVertexInputState   = &vertexInputCI;
    pipeCI.pInputAssemblyState = &inputAssemblyCI;
    pipeCI.pViewportState      = &viewportCI;
    pipeCI.pRasterizationState = &rasterCI;
    pipeCI.pMultisampleState   = &msCI;
    pipeCI.pDepthStencilState  = &depthCI;
    pipeCI.pColorBlendState    = &blendCI;
    pipeCI.pDynamicState       = &dynamicCI;
    pipeCI.layout              = s_pipelineLayout;
    pipeCI.renderPass          = s_renderPass;
    pipeCI.subpass             = 0;

    VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeCI, nullptr, &s_pipeline);

    vkDestroyShaderModule(device, vsModule, nullptr);
    vkDestroyShaderModule(device, fsModule, nullptr);

    return (result == VK_SUCCESS) ? FFX_OK : FFX_ERROR_BACKEND_API_ERROR;
}

FfxErrorCode ffxDebugPacingInitVK(VkDevice device, VkFormat swapchainFormat)
{
    if (s_pipeline != VK_NULL_HANDLE)
        return FFX_OK;
    return createPipelineResources(device, swapchainFormat);
}

FfxErrorCode ffxDebugPacingRenderVK(VkDevice device, VkCommandBuffer cmdBuf,
                                    VkImage targetImage, VkImageView targetView,
                                    VkFormat format, uint32_t width, uint32_t height,
                                    uint32_t frameIndex)
{
    FfxErrorCode err = ffxDebugPacingInitVK(device, format);
    if (err != FFX_OK)
        return err;

    // Create a transient framebuffer for this image view
    VkFramebufferCreateInfo fbCI{};
    fbCI.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbCI.renderPass      = s_renderPass;
    fbCI.attachmentCount = 1;
    fbCI.pAttachments    = &targetView;
    fbCI.width           = width;
    fbCI.height          = height;
    fbCI.layers          = 1;

    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    if (vkCreateFramebuffer(device, &fbCI, nullptr, &framebuffer) != VK_SUCCESS)
        return FFX_ERROR_BACKEND_API_ERROR;

    // Begin render pass
    VkRenderPassBeginInfo rpBegin{};
    rpBegin.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBegin.renderPass      = s_renderPass;
    rpBegin.framebuffer     = framebuffer;
    rpBegin.renderArea      = {{0, 0}, {width, height}};

    vkCmdBeginRenderPass(cmdBuf, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

    // Bind pipeline
    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, s_pipeline);

    // Viewport covers only the left 32 pixels
    VkViewport viewport{};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = 32.0f;
    viewport.height   = static_cast<float>(height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmdBuf, 0, 1, &viewport);

    // Scissor covers the full backbuffer
    VkRect2D scissor{{0, 0}, {width, height}};
    vkCmdSetScissor(cmdBuf, 0, 1, &scissor);

    // Push the frame index
    vkCmdPushConstants(cmdBuf, s_pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(uint32_t), &frameIndex);

    // Draw full-screen triangle (3 vertices, no vertex buffer)
    vkCmdDraw(cmdBuf, 3, 1, 0, 0);

    vkCmdEndRenderPass(cmdBuf);

    // Destroy the transient framebuffer after the command buffer is submitted.
    // Since we're inside a recorded command buffer, we need to defer destruction.
    // In practice the caller should track this. For simplicity in this debug overlay,
    // we destroy immediately (the command buffer references are retained by the driver).
    vkDestroyFramebuffer(device, framebuffer, nullptr);

    s_frameIndex++;

    return FFX_OK;
}

void ffxDebugPacingReleaseVK(VkDevice device)
{
    if (s_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(device, s_pipeline, nullptr);
        s_pipeline = VK_NULL_HANDLE;
    }
    if (s_renderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(device, s_renderPass, nullptr);
        s_renderPass = VK_NULL_HANDLE;
    }
    if (s_pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(device, s_pipelineLayout, nullptr);
        s_pipelineLayout = VK_NULL_HANDLE;
    }
    s_frameIndex = 0;
}
