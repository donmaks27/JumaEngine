// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "VertexBufferObject_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "RenderOptions_Vulkan.h"
#include "RenderSubsystem_Vulkan.h"
#include "jutils/jset.h"
#include "subsystems/render/vertex/VertexBufferData.h"
#include "vulkanObjects/VulkanBuffer.h"
#include "vulkanObjects/VulkanCommandBuffer.h"
#include "vulkanObjects/VulkanQueueType.h"
#include "vulkanObjects/VulkanRenderImage.h"

namespace JumaEngine
{
    VertexBufferObject_Vulkan::~VertexBufferObject_Vulkan()
    {
        clearVulkanData();
    }

    bool VertexBufferObject_Vulkan::initInternal()
    {
        RenderSubsystem_Vulkan* renderSubsystem = getRenderSubsystem();
        const VertexBufferDataBase* vertexData = getVertexData();
        const jset<VulkanQueueType> queues = { VulkanQueueType::Graphics, VulkanQueueType::Transfer };

        m_VertexBuffer = renderSubsystem->createVulkanObject<VulkanBuffer>();
        bool success = m_VertexBuffer->initGPUBuffer(
            vertexData->getVertices(), vertexData->getVertexSize() * m_Parent->getVertexCount(), 
            queues, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        );
        if (!success)
        {
            JUMA_LOG(error, JSTR("Failed to initialize vertex buffer"));
            clearVulkanData();
            return false;
        }

        const uint32 indexCount = m_Parent->getIndexCount();
        if (indexCount > 0)
        {
            m_IndexBuffer = renderSubsystem->createVulkanObject<VulkanBuffer>();
            success = m_IndexBuffer->initGPUBuffer(
                vertexData->getIndices(), sizeof(uint32) * indexCount, queues, VK_BUFFER_USAGE_INDEX_BUFFER_BIT
            );
            if (!success)
            {
                JUMA_LOG(error, JSTR("Failed to initialize index buffer"));
                clearVulkanData();
                return false;
            }
        }

        renderSubsystem->registerVertexType(vertexData);
        return true;
    }

    void VertexBufferObject_Vulkan::clearVulkanData()
    {
        if (m_IndexBuffer != nullptr)
        {
            delete m_IndexBuffer;
            m_IndexBuffer = nullptr;
        }
        if (m_VertexBuffer != nullptr)
        {
            delete m_VertexBuffer;
            m_VertexBuffer = nullptr;
        }
    }

    void VertexBufferObject_Vulkan::draw(VkCommandBuffer commandBuffer)
    {
        VkBuffer vertexBuffer = m_VertexBuffer->get();
        const VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);

        if (m_IndexBuffer == nullptr)
        {
            vkCmdDraw(commandBuffer, m_Parent->getVertexCount(), 1, 0, 0);
        }
        else
        {
            vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->get(), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffer, m_Parent->getIndexCount(), 1, 0, 0, 0);
        }
    }

    bool VertexBufferObject_Vulkan::render(const RenderOptions* renderOptions)
    {
        if (renderOptions == nullptr)
        {
            JUMA_LOG(error, JSTR("Invalid input params"));
            return false;
        }

        const RenderOptions_Vulkan* options = reinterpret_cast<const RenderOptions_Vulkan*>(renderOptions);
        const VulkanCommandBuffer* commandBufferObject = options->renderImage != nullptr ? options->renderImage->getRenderCommandBuffer() : nullptr;
        VkCommandBuffer commandBuffer = commandBufferObject != nullptr ? commandBufferObject->get() : nullptr;
        if (commandBuffer == nullptr)
        {
            JUMA_LOG(error, JSTR("Invalid command buffer"));
            return true;
        }

        draw(commandBuffer);
        return true;
    }
}

#endif
