// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "jutils/jarray.h"
#include "jutils/jmap.h"
#include "jutils/math/vector2.h"
#include "jutils/jstringID.h"

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
        bool init(VulkanRenderPass* renderPass, const math::uvector2& framebufferSize);

        void setDependency(const jstringID& name, VulkanRenderImage* renderImage);
        void removeDependency(const jstringID& name);
        void clearDependencies();

        bool update();

        VulkanRenderPass* getRenderPass() const { return m_RenderPass; }
        VkFence getFinishFence() const;
        VkSemaphore getFinishSemaphore() const;

        VulkanCommandBuffer* startRenderCommandBufferRecord();
        VulkanCommandBuffer* getRenderCommandBuffer() const;
        bool submitRenderCommandBuffer();

        VulkanFramebuffer* getFramebuffer(const int8 frameIndex) const { return m_Framebuffers.isValidIndex(frameIndex) ? m_Framebuffers[frameIndex] : nullptr; }

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

        jmap<jstringID, VulkanRenderImage*> m_Dependencies;


        void clearVulkan();

        bool updateRenderFrameCount();
        bool updateFramebufferCount();
        bool updateDependencies();

        int8 getCurrentFramebufferIndex() const;
        int8 getCurrentRenderFrameIndex() const;
    };
}

#endif
