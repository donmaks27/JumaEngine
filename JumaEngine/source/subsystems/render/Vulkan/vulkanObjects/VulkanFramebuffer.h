// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "VulkanImage.h"
#include "jutils/math/vector2.h"

namespace JumaEngine
{
    class VulkanSwapchain;
    class VulkanRenderPass;

    class VulkanFramebuffer : public VulkanContextObject
    {
    public:
        VulkanFramebuffer() = default;
        virtual ~VulkanFramebuffer() override;

        bool update(const VulkanSwapchain* swapchain, int8 swapchainImageIndex);
        bool update(VulkanRenderPass* renderPass, const math::uvector2& size);
        
        VkFramebuffer get() const { return m_Framebuffer; }
        VulkanRenderPass* getRenderPass() const { return m_RenderPass; }
        math::uvector2 getSize() const { return m_ColorAttachmentImage != nullptr ? m_ColorAttachmentImage->getSize() : math::uvector2{ 0, 0 }; }
        VulkanImage* getResultImage() const { return m_ExportAvailable ? (m_ResolveAttachmentImage != nullptr ? m_ResolveAttachmentImage : m_ColorAttachmentImage) : nullptr; }
        
        bool startRender(VulkanCommandBuffer* commandBuffer);
        bool finishRender(VulkanCommandBuffer* commandBuffer);

    protected:

        virtual void clearInternal() override { clearData(); }

    private:

        VkFramebuffer m_Framebuffer = nullptr;

        VulkanRenderPass* m_RenderPass = nullptr;
        VulkanImage* m_ColorAttachmentImage = nullptr;
        VulkanImage* m_DepthAttachmentImage = nullptr;
        VulkanImage* m_ResolveAttachmentImage = nullptr;
        bool m_ExportAvailable = false;


        void clearData();
        void clearFramebuffer();

        bool update(VulkanRenderPass* renderPass, const math::uvector2& size, VkImage resultImage);
        bool recreateImages(const math::uvector2& size, VkImage resultImage);
        bool createFramebuffer(const math::uvector2& size);
    };
}

#endif
