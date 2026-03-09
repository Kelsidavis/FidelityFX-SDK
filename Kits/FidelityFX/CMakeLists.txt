# This file is part of the FidelityFX SDK.
#
# Copyright (C) 2025 Advanced Micro Devices, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

cmake_minimum_required(VERSION 3.16)
project(FidelityFX VERSION 2.1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(FFX_BUILD_VK "Build Vulkan backend" ON)
option(FFX_BUILD_FRAMEGENERATION "Build frame generation support" ON)
option(FFX_BUILD_UPSCALER "Build upscaler support" OFF)

# Find Vulkan
if(FFX_BUILD_VK)
    find_package(Vulkan REQUIRED)
endif()

# ---------------------------------------------------------------------------
# Shader permutation header output directory
# Run BuildShaders_VK.sh or the .bat scripts to populate this before building.
# The FidelityFX_SC tool (or DXC with -spirv) generates permutation headers
# that are #included by the shader blob .cpp files.
# ---------------------------------------------------------------------------
set(FFX_SC_OUTPUT_DIR "${CMAKE_BINARY_DIR}/ffx_sc_output" CACHE PATH
    "Directory containing generated shader permutation headers")

# ============================================================================
# VK Backend Library
# ============================================================================
if(FFX_BUILD_VK)
    # --- Core API sources (backend-agnostic) ---
    set(FFX_API_SOURCES
        api/internal/ffx_api.cpp
        api/internal/ffx_assert.cpp
        api/internal/ffx_message.cpp
        api/internal/ffx_object_management.cpp
        api/internal/ffx_query_fallback.cpp
    )

    # --- VK backend sources ---
    set(FFX_VK_BACKEND_SOURCES
        backend/vk/ffx_vk.cpp
        backend/vk/ffx_backends_vk.cpp
    )

    # --- Frame generation sources (backend-agnostic + VK-specific) ---
    set(FFX_FRAMEGENERATION_SOURCES "")
    if(FFX_BUILD_FRAMEGENERATION)
        list(APPEND FFX_FRAMEGENERATION_SOURCES
            framegeneration/fsr3/internal/ffx_frameinterpolation.cpp
            framegeneration/fsr3/internal/ffx_frameinterpolation_shaderblobs.cpp
            framegeneration/fsr3/internal/ffx_opticalflow.cpp
            framegeneration/fsr3/internal/ffx_opticalflow_shaderblobs.cpp
            framegeneration/fsr3/internal/ffx_provider_fsr3framegeneration.cpp
            framegeneration/fsr3/vk/ffx_provider_fsr3framegenerationswapchain_vk.cpp
            framegeneration/fsr3/vk/FrameInterpolationSwapchainVK.cpp
            framegeneration/fsr3/vk/FrameInterpolationSwapchainVK_Helpers.cpp
            framegeneration/fsr3/vk/FrameInterpolationSwapchainVK_UiComposition.cpp
        )
    endif()

    # --- Upscaler sources ---
    set(FFX_UPSCALER_SOURCES "")
    if(FFX_BUILD_UPSCALER)
        file(GLOB FFX_UPSCALER_SOURCES_FOUND
            upscalers/fsr3/internal/*.cpp
        )
        list(APPEND FFX_UPSCALER_SOURCES ${FFX_UPSCALER_SOURCES_FOUND})
    endif()

    # --- Build the VK shared library ---
    add_library(amd_fidelityfx_vk SHARED
        ${FFX_API_SOURCES}
        ${FFX_VK_BACKEND_SOURCES}
        ${FFX_FRAMEGENERATION_SOURCES}
        ${FFX_UPSCALER_SOURCES}
    )

    target_include_directories(amd_fidelityfx_vk PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/api/include
        ${CMAKE_CURRENT_SOURCE_DIR}/api/internal
        ${CMAKE_CURRENT_SOURCE_DIR}/api/internal/include/gpu
        ${CMAKE_CURRENT_SOURCE_DIR}/framegeneration/include
        ${CMAKE_CURRENT_SOURCE_DIR}/framegeneration/fsr3/include
        ${CMAKE_CURRENT_SOURCE_DIR}/framegeneration/fsr3/include/gpu
        ${CMAKE_CURRENT_SOURCE_DIR}/framegeneration/fsr3/internal
        ${CMAKE_CURRENT_SOURCE_DIR}/framegeneration/fsr3/vk
        ${CMAKE_CURRENT_SOURCE_DIR}/framegeneration/fsr3/dx12  # shared headers
        ${CMAKE_CURRENT_SOURCE_DIR}/backend/vk
        ${Vulkan_INCLUDE_DIRS}
        # Generated shader permutation headers
        ${FFX_SC_OUTPUT_DIR}
    )

    target_compile_definitions(amd_fidelityfx_vk PRIVATE
        FFX_BACKEND_VK=1
        $<$<BOOL:${FFX_BUILD_FRAMEGENERATION}>:FFX_FRAMEGENERATION=1>
        $<$<BOOL:${FFX_BUILD_UPSCALER}>:FFX_UPSCALER=1>
    )

    target_link_libraries(amd_fidelityfx_vk PRIVATE
        Vulkan::Vulkan
    )

    # Export only the FFX API entry points
    if(NOT WIN32)
        set_target_properties(amd_fidelityfx_vk PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
        )
    endif()

    set_target_properties(amd_fidelityfx_vk PROPERTIES
        OUTPUT_NAME "amd_fidelityfx_vk"
        VERSION ${PROJECT_VERSION}
        SOVERSION 2
    )

    # Install
    install(TARGETS amd_fidelityfx_vk
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )

    # Install public headers
    install(DIRECTORY api/include/ DESTINATION include/ffx
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
    )
    install(DIRECTORY framegeneration/include/ DESTINATION include/ffx
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
    )
endif()
