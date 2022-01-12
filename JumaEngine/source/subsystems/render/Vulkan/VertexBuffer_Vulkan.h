// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/vertexBuffer/VertexBuffer.h"
#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "jutils/jshared_ptr.h"

namespace JumaEngine
{
    struct RenderOptionsData_Vulkan;
    class VulkanCommandBuffer;
    class VulkanBuffer;

    class VertexBuffer_Vulkan final : public VertexBuffer, public VulkanContextObjectBase
    {
        JUMAENGINE_CLASS(VertexBuffer_Vulkan, VertexBuffer)

    public:
        VertexBuffer_Vulkan() = default;
        virtual ~VertexBuffer_Vulkan() override;

        void render(const RenderOptionsData_Vulkan& data);

        const jarray<VkVertexInputBindingDescription>& getBindingDescriptions() const { return m_BindingDescriptions; }
        const jarray<VkVertexInputAttributeDescription>& getAttributeDescriptions() const { return m_AttributeDescriptions; }

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
