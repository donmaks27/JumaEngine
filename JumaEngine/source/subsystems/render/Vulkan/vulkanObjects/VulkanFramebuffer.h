// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "jutils/jarray.h"
#include "jutils/jshared_ptr.h"
#include "jutils/math/vector2.h"

namespace JumaEngine
{
    class VulkanCommandBuffer;
    class Image_Vulkan;
    class VulkanRenderPass;

    class VulkanFramebuffer : public VulkanContextObject
    {
    public:
        VulkanFramebuffer() = default;
        virtual ~VulkanFramebuffer() override;

        bool create(VulkanRenderPass* renderPass, const math::uvector2& size, const jshared_ptr<Image_Vulkan>& resultImage);
        bool create(VulkanRenderPass* renderPass, const math::uvector2& size, VkImage resultImage);

        VkFramebuffer get() const { return m_Framebuffer; }
        VulkanRenderPass* getRenderPass() const { return m_RenderPass; }

        const jshared_ptr<VulkanCommandBuffer>& getRenderCommandBuffer() const { return m_CommandBuffer; }
        void setRenderCommandBuffer(const jshared_ptr<VulkanCommandBuffer>& commandBuffer) { m_CommandBuffer = commandBuffer; }

    protected:

        virtual void clearInternal() override { clearFramebuffer(); }

    private:

        VkFramebuffer m_Framebuffer = nullptr;

        VulkanRenderPass* m_RenderPass = nullptr;

        jarray<jshared_ptr<Image_Vulkan>> m_Images;
        math::uvector2 m_ImagesSize = { 0, 0 };
        int32 m_ResultImageIndex = -1;
        
        jshared_ptr<VulkanCommandBuffer> m_CommandBuffer = nullptr;

        
        bool init(VulkanRenderPass* renderPass, const math::uvector2& size, const jshared_ptr<Image_Vulkan>& resultImage);
        bool recreate(VulkanRenderPass* renderPass, const math::uvector2& size, const jshared_ptr<Image_Vulkan>& resultImage);

        bool createImages(const jshared_ptr<Image_Vulkan>& resultImage);
        bool createFrambuffer();

        void clearFramebuffer();
    };
}

#endif
