// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "jutils/jshared_ptr.h"
#include "jutils/math/vector2.h"

namespace JumaEngine
{
    class VulkanImage;
    class VulkanCommandBuffer;
    class VulkanRenderPass;

    class VulkanFramebuffer : public VulkanContextObject
    {
    public:
        VulkanFramebuffer() = default;
        virtual ~VulkanFramebuffer() override;

        bool create(VulkanRenderPass* renderPass, const math::uvector2& size, VkImage resultImage = nullptr);

        VkFramebuffer get() const { return m_Framebuffer; }
        VulkanRenderPass* getRenderPass() const { return m_RenderPass; }

        const jshared_ptr<VulkanCommandBuffer>& getRenderCommandBuffer() const { return m_CommandBuffer; }
        void setRenderCommandBuffer(const jshared_ptr<VulkanCommandBuffer>& commandBuffer) { m_CommandBuffer = commandBuffer; }

    protected:

        virtual void clearInternal() override { clearFramebuffer(); }

    private:

        VkFramebuffer m_Framebuffer = nullptr;

        VulkanRenderPass* m_RenderPass = nullptr;

        math::uvector2 m_ImagesSize = { 0, 0 };
        VulkanImage* m_ColorImage = nullptr;
        VulkanImage* m_DepthImage = nullptr;
        VulkanImage* m_ResolveImage = nullptr;
        
        jshared_ptr<VulkanCommandBuffer> m_CommandBuffer = nullptr;

        
        bool init(VulkanRenderPass* renderPass, const math::uvector2& size, VkImage resultImage);
        bool recreate(VulkanRenderPass* renderPass, const math::uvector2& size, VkImage resultImage);

        bool recreateImages(VkImage resultImage);
        bool createFrambuffer();

        void clearFramebuffer();
    };
}

#endif
