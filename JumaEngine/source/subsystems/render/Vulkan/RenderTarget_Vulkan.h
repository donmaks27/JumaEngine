// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderTarget.h"
#include "VulkanContextObject.h"

namespace JumaEngine
{
    class VulkanSwapchain;
    class VulkanFramebuffer;
    class VulkanRenderPass;
    class VulkanCommandBuffer;
    class VulkanRenderImage;

    class RenderTarget_RenderAPIObject_Vulkan : public RenderTarget_RenderAPIObject, public VulkanContextObjectBase
    {
    public:
        RenderTarget_RenderAPIObject_Vulkan() = default;
        virtual ~RenderTarget_RenderAPIObject_Vulkan() override;

        VulkanRenderPass* getRenderPass() const { return m_RenderPass; }
        VulkanFramebuffer* getFramebuffer(const int8 frameIndex) const { return m_Framebuffers.isValidIndex(frameIndex) ? m_Framebuffers[frameIndex] : nullptr; }

        bool startRender(VulkanCommandBuffer* commandBuffer);
        bool finishRender(VulkanCommandBuffer* commandBuffer);

    protected:

        virtual bool initInternal() override;

    private:

        VulkanSwapchain* m_Swapchain = nullptr;
        VulkanRenderPass* m_RenderPass = nullptr;
        jarray<VulkanFramebuffer*> m_Framebuffers;


        void clearData();

        bool initWindowRenderTarget();
        bool initRenderTarget();

        bool updateFramebuffers();
        int8 getFramebufferCount() const;
        int8 getRenderFrameCount() const;
        int8 getCurrentFramebufferIndex() const;
        int8 getCurrentRenderFrameIndex() const;
    };
}

#endif
