// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "jutils/jarray.h"
#include "jutils/math/vector2.h"

namespace JumaEngine
{
    class VulkanRenderPass;
    class VulkanSwapchain;
    class VulkanFramebuffer;
    class VulkanCommandBuffer;

    class VulkanRenderImage : public VulkanContextObject
    {
    public:
        VulkanRenderImage() = default;
        virtual ~VulkanRenderImage() override;

        bool init(VulkanSwapchain* swapchain);

        bool update();
        bool setRenderFrameIndex(int8 renderFrameIndex);

        VulkanRenderPass* getRenderPass() const { return m_RenderPass; }
        VkFence getFinishFence() const { return m_RenderFrames.isValidIndex(m_CurrentRenderFrameIndex) ? m_RenderFrames[m_CurrentRenderFrameIndex].finishFence : nullptr; }
        VkSemaphore getFinishSemaphore() const { return m_RenderFrames.isValidIndex(m_CurrentRenderFrameIndex) ? m_RenderFrames[m_CurrentRenderFrameIndex].finishSemaphore : nullptr; }

        VulkanCommandBuffer* startRenderCommandBufferRecord();
        VulkanCommandBuffer* getRenderCommandBuffer() const { return m_RenderFrames.isValidIndex(m_CurrentRenderFrameIndex) ? m_RenderFrames[m_CurrentRenderFrameIndex].commandBuffer : nullptr; }
        bool submitRenderCommandBuffer();

    protected:

        virtual void clearInternal() override { clearVulkan(); }

    private:

        struct RenderFrame
        {
            VkFence finishFence = nullptr;
            VkSemaphore finishSemaphore = nullptr;

            jarray<VkSemaphore> startSemaphores;

            VulkanCommandBuffer* commandBuffer = nullptr;
        };

        jarray<VulkanFramebuffer*> m_Framebuffers;
        jarray<RenderFrame> m_RenderFrames;

        VulkanSwapchain* m_Swapchain = nullptr;
        VulkanRenderPass* m_RenderPass = nullptr;
        math::uvector2 m_FramebufferSize = { 0, 0 };

        int8 m_CurrentFramebufferIndex = -1;
        int8 m_CurrentRenderFrameIndex = -1;


        void clearVulkan();

        int8 getDesiredRenderFrameCount() const;

        bool updateRenderFrameCount();
        bool updateFramebufferCount();
        bool updateDependencies();
    };
}

#endif
