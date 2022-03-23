// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderTarget.h"
#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "jutils/jarray.h"
#include "jutils/math/vector2.h"

namespace JumaEngine
{
    class VulkanRenderPass;
    class VulkanFramebuffer;

    class RenderTarget_Vulkan : public RenderTarget, public VulkanContextObjectBase
    {
        JUMAENGINE_CLASS(RenderTarget_Vulkan, RenderTarget)

    public:
        RenderTarget_Vulkan() = default;
        virtual ~RenderTarget_Vulkan() override = default;

    private:

        struct FramebufferContainer
        {
            VulkanFramebuffer* framebuffer = nullptr;
            int8 renderFrameIndex = -1;
        };
        struct RenderFrame
        {
            VkFence renderFinishedFence = nullptr;
            VkSemaphore renderFinishedSemaphore = nullptr;

            jarray<VkSemaphore> startSemaphores;
        };

        VulkanRenderPass* m_RenderPass = nullptr;
        math::uvector2 m_FramebufferSize = { 0, 0 };

        jarray<FramebufferContainer> m_Framebuffers;
        jarray<RenderFrame> m_RenderFrames;

        uint8 m_MaxRenderFrameCount = 0;
        uint8 m_CurrentRenderFrameIndex = 0;


        bool createFramebuffers(uint8 framebufferCount, VulkanRenderPass* renderPass, const math::uvector2& size);
        bool createRenderFrames(uint8 frameCount);
    };
}

#endif
