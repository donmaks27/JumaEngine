// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "VulkanContextObject.h"
#include "subsystems/render/VertexBuffer.h"

#include <vulkan/vulkan_core.h>

namespace JumaEngine
{
    class VulkanBuffer;

    class VertexBufferRenderAPIObject_Vulkan : public VertexBufferRenderAPIObject, public VulkanContextObjectBase
    {
    public:
        VertexBufferRenderAPIObject_Vulkan() = default;
        virtual ~VertexBufferRenderAPIObject_Vulkan() override;

        virtual bool render(const RenderOptions* renderOptions) override;

    protected:

        virtual bool initInternal() override;

    private:

        VulkanBuffer* m_VertexBuffer = nullptr;
        VulkanBuffer* m_IndexBuffer = nullptr;


        void clearVulkanData();

        void draw(VkCommandBuffer commandBuffer);
    };
}

#endif
