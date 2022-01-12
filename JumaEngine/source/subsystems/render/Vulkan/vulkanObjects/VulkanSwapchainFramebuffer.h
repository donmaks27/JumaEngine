// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "jutils/jshared_ptr.h"

namespace JumaEngine
{
    class VulkanCommandBuffer;
    class Image_Vulkan;
    class VulkanSwapchain;

    class VulkanSwapchainFramebuffer : public VulkanContextObject
    {
    public:
        VulkanSwapchainFramebuffer() = default;
        virtual ~VulkanSwapchainFramebuffer() override;

        bool init(VulkanSwapchain* swapchain, VkImage swapchainImage);

        const jshared_ptr<Image_Vulkan>& getImage() const { return m_Image; }
        VkFramebuffer get() const { return m_Framebuffer; }

        VkFence getRenderFinishedFence() const { return m_Fence_RenderFinished; }
        void setRenderFinishedFence(VkFence fence) { m_Fence_RenderFinished = fence; }

        const jshared_ptr<VulkanCommandBuffer>& getCommandBuffer() const { return m_CommandBuffer; }
        void setCommandBuffer(const jshared_ptr<VulkanCommandBuffer>& buffer) { m_CommandBuffer = buffer; }

    protected:

        virtual void clearInternal() override { clearFramebuffer(); }

    private:

        VulkanSwapchain* m_ParentSwapchain = nullptr;
        jshared_ptr<Image_Vulkan> m_Image = nullptr;
        VkFramebuffer m_Framebuffer = nullptr;

        VkFence m_Fence_RenderFinished = nullptr;
        jshared_ptr<VulkanCommandBuffer> m_CommandBuffer = nullptr;


        void clearFramebuffer();
    };
}

#endif
