// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderOptions.h"

namespace JumaEngine
{
    class VulkanCommandBuffer;
    class VulkanFramebuffer;

    struct RenderOptions_Vulkan : public RenderOptions
    {
        uint32 frameIndex = 0;
        uint32 swapchainImageIndex = 0;
        VulkanFramebuffer* framebuffer = nullptr;
        VulkanCommandBuffer* commandBuffer = nullptr;
    };
}

#endif
