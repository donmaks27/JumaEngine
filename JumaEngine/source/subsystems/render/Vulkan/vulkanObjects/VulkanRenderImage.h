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
    class VulkanCommandBuffer;
    class VulkanSwapchain;
    class VulkanRenderPass;
    class VulkanFramebuffer;

    class VulkanRenderImage : public VulkanContextObject
    {
    public:
        VulkanRenderImage() = default;
        virtual ~VulkanRenderImage() override;

        bool init(VulkanSwapchain* swapchain);
        bool init(VulkanRenderPass* renderPass, const math::uvector2& framebufferSize);

        bool update();

        VulkanRenderPass* getRenderPass() const { return m_RenderPass; }
        const math::uvector2& getSize() const { return m_FramebufferSize; }

        VulkanFramebuffer* getFramebuffer(const int8 frameIndex) const { return m_Framebuffers.isValidIndex(frameIndex) ? m_Framebuffers[frameIndex] : nullptr; }
        
        bool startRender(VulkanCommandBuffer* commandBuffer);
        bool finishRender(VulkanCommandBuffer* commandBuffer);
        
    protected:

        virtual void clearInternal() override { clearVulkan(); }

    private:

        jarray<VulkanFramebuffer*> m_Framebuffers;

        VulkanSwapchain* m_Swapchain = nullptr;
        VulkanRenderPass* m_RenderPass = nullptr;
        math::uvector2 m_FramebufferSize = { 0, 0 };


        void clearVulkan();

        int8 getFramebufferCount() const;
        int8 getRenderFrameCount() const;
        int8 getCurrentFramebufferIndex() const;
        int8 getCurrentRenderFrameIndex() const;

        bool updateFramebufferCount();
    };
}

#endif
