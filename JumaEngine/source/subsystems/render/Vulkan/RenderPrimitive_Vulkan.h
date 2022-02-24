// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderPrimitive.h"
#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

namespace JumaEngine
{
    class VulkanSwapchain;

    class RenderPrimitive_Vulkan : public RenderPrimitive, public VulkanContextObjectBase
    {
        JUMAENGINE_CLASS(RenderPrimitive_Vulkan, RenderPrimitive)

    public:
        RenderPrimitive_Vulkan() = default;
        virtual ~RenderPrimitive_Vulkan() override;

        virtual void render(const RenderOptionsOld& options) override;

    protected:

        virtual bool initInternal(const jshared_ptr<VertexBufferOld>& vertexBuffer, const jshared_ptr<MaterialOld>& material) override;
        virtual void clearInternal() override;

    private:

        bool m_RenderPipelineInvaid = false;
        VkPipeline m_RenderPipeline = nullptr;


        bool createRenderPipeline(const jshared_ptr<VertexBufferOld>& vertexBuffer, const jshared_ptr<MaterialOld>& material);
        bool recreateRenderPipeline();
        void onRenderPassChanged(VulkanSwapchain* swapchain);

        void clearVulkanData();
        void clearRenderPipeline();
    };
}

#endif
