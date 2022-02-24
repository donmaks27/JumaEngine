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

    class VertexBufferObject_Vulkan : public VertexBufferObject, public VulkanContextObject
    {
    public:
        VertexBufferObject_Vulkan() = default;
        virtual ~VertexBufferObject_Vulkan() override;

        virtual bool render(const RenderOptions* renderOptions) override;

    protected:

        virtual bool initInternal() override;
        virtual void clearInternal() override { clearVulkanData(); }

    private:

        VulkanBuffer* m_VertexBuffer = nullptr;
        VulkanBuffer* m_IndexBuffer = nullptr;


        void clearVulkanData();

        void draw(VkCommandBuffer commandBuffer);
    };
}

#endif
