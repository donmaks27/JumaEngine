// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/VertexBuffer.h"
#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "utils/jshared_ptr.h"

namespace JumaEngine
{
    class VulkanCommandBuffer;
    class VulkanBuffer;

    class VertexBuffer_Vulkan final : public VertexBuffer, public VulkanContextObjectBase
    {
        JUMAENGINE_CLASS(VertexBuffer_Vulkan, VertexBuffer)

    public:
        VertexBuffer_Vulkan() = default;
        virtual ~VertexBuffer_Vulkan() override;

        void render(const jshared_ptr<VulkanCommandBuffer>& commandBuffer);

    protected:

        virtual bool initInternal(const VertexBufferDataBase* data) override;
        virtual void clearInternal() override;

    private:

        jshared_ptr<VulkanBuffer> m_VertexBuffer = nullptr;
        jshared_ptr<VulkanBuffer> m_IndexBuffer = nullptr;

        jarray<VkVertexInputBindingDescription> m_BindingDescriptions;
        jarray<VkVertexInputAttributeDescription> m_AttributeDescriptions;


        void clearVulkanBuffers();
    };
}

#endif
