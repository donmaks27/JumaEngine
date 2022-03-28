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

        VulkanRenderPass* getRenderPass() const { return m_RenderPass; }
        VkFence getFinishFence() const;
        VkSemaphore getFinishSemaphore() const;

        VulkanCommandBuffer* startRenderCommandBufferRecord();
        VulkanCommandBuffer* getRenderCommandBuffer() const;
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


        void clearVulkan();

        bool updateRenderFrameCount();
        bool updateFramebufferCount();
        bool updateDependencies();

        int8 getCurrentFramebufferIndex() const;
        int8 getCurrentRenderFrameIndex() const;
    };
}

#endif
