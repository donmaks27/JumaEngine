// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderTarget.h"
#include "VulkanContextObject.h"

namespace JumaEngine
{
    class VulkanFramebuffer;
    class VulkanRenderPass;
    class VulkanCommandBuffer;
    class VulkanRenderImage;

    class RenderTarget_RenderAPIObject_Vulkan : public RenderTarget_RenderAPIObject, public VulkanContextObjectBase
    {
    public:
        RenderTarget_RenderAPIObject_Vulkan() = default;
        virtual ~RenderTarget_RenderAPIObject_Vulkan() override;

        VulkanRenderPass* getRenderPass() const;
        VulkanFramebuffer* getFramebuffer(int8 frameIndex) const;

        bool startRender(VulkanCommandBuffer* commandBuffer);
        bool finishRender(VulkanCommandBuffer* commandBuffer);

    protected:

        virtual bool initInternal() override { return m_Parent->isWindowRenderTarget() ? initWindowRenderTarget() : initRenderTarget(); }

    private:

        VulkanRenderImage* m_RenderImage = nullptr;


        void clearData();

        bool initWindowRenderTarget();
        bool initRenderTarget();
    };
}

#endif
