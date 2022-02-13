// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexBuffer_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "RenderOptionsData_Vulkan.h"
#include "RenderSubsystem_Vulkan.h"
#include "subsystems/render/vertexBuffer/VertexBufferData.h"
#include "vulkanObjects/VulkanBuffer.h"
#include "vulkanObjects/VulkanCommandBuffer.h"

namespace JumaEngine
{
    VertexBuffer_Vulkan::~VertexBuffer_Vulkan()
    {
        if (isValid())
        {
            clearVulkanBuffers();
        }
    }

    bool VertexBuffer_Vulkan::initInternal(const VertexBufferDataBase* data)
    {
        const uint32 vertexSize = getVertexSize();
        const uint32 indexCount = getIndexCount();
        const jarray<VertexComponentDescription>& vertexComponents = getVertexComponents();

        const jset<VulkanQueueType> queues = { VulkanQueueType::Graphics, VulkanQueueType::Transfer };

        m_VertexBuffer = getRenderSubsystem()->createVulkanObject<VulkanBuffer>();
        m_VertexBuffer->initGPUBuffer(
            data->getVertices(), vertexSize * getVertexCount(), 
            queues, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        );
        if (!m_VertexBuffer->isValid())
        {
            m_VertexBuffer = nullptr;
            return false;
        }

        if (indexCount > 0)
        {
            m_IndexBuffer = getRenderSubsystem()->createVulkanObject<VulkanBuffer>();
            m_IndexBuffer->initGPUBuffer(
                data->getIndices(), sizeof(uint32) * indexCount, 
                queues, VK_BUFFER_USAGE_INDEX_BUFFER_BIT
            );
            if (!m_IndexBuffer->isValid())
            {
                m_VertexBuffer = nullptr;
                m_IndexBuffer = nullptr;
                return false;
            }
        }

        getRenderSubsystem()->registerVertexType(data);
        m_VertexName = data->getVertexName();
        return true;
    }

    void VertexBuffer_Vulkan::clearInternal()
    {
        clearVulkanBuffers();
    }
    void VertexBuffer_Vulkan::clearVulkanBuffers()
    {
        m_VertexName = jstringID_NONE;

        m_VertexBuffer.reset();
        m_IndexBuffer.reset();
    }

    void VertexBuffer_Vulkan::render(const RenderOptionsData_Vulkan& data)
    {
        if (!isValid())
        {
            return;
        }

        VkCommandBuffer vulkanCommandBuffer = data.commandBuffer->get();

        VkDeviceSize offsets[] = { 0 };
        VkBuffer vertexBuffer = m_VertexBuffer->get();
        vkCmdBindVertexBuffers(vulkanCommandBuffer, 0, 1, &vertexBuffer, offsets);

        if (m_IndexBuffer == nullptr)
        {
            vkCmdDraw(vulkanCommandBuffer, getVertexCount(), 1, 0, 0);
        }
        else
        {
            vkCmdBindIndexBuffer(vulkanCommandBuffer, m_IndexBuffer->get(), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(vulkanCommandBuffer, getIndexCount(), 1, 0, 0, 0);
        }
    }
}

#endif
