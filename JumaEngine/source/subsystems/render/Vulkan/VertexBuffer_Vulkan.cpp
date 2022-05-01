// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "VertexBuffer_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "RenderOptions_Vulkan.h"
#include "RenderSubsystem_Vulkan.h"
#include "jutils/jset.h"
#include "subsystems/render/vertex/VertexBufferData.h"
#include "vulkanObjects/VulkanBuffer.h"
#include "vulkanObjects/VulkanCommandBuffer.h"
#include "vulkanObjects/VulkanQueueType.h"

namespace JumaEngine
{
    VertexBuffer_RenderAPIObject_Vulkan::~VertexBuffer_RenderAPIObject_Vulkan()
    {
        clearVulkanData();
    }

    bool VertexBuffer_RenderAPIObject_Vulkan::initInternal()
    {
        RenderSubsystem_RenderAPIObject_Vulkan* renderSubsystem = getRenderSubsystemObject();
        const VertexBufferDataBase* vertexData = getVertexData();
        const VertexDescription* vertexDescription = getRenderSubsystemObject()->getParent()->findVertexDescription(vertexData->getVertexName());
        const jset<VulkanQueueType> queues = { VulkanQueueType::Graphics, VulkanQueueType::Transfer };

        m_VertexBuffer = renderSubsystem->createVulkanObject<VulkanBuffer>();
        bool success = m_VertexBuffer->initGPUBuffer(
            vertexData->getVertices(), vertexDescription->size * m_Parent->getVertexCount(), 
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
        return true;
    }

    void VertexBuffer_RenderAPIObject_Vulkan::clearVulkanData()
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

    void VertexBuffer_RenderAPIObject_Vulkan::draw(VkCommandBuffer commandBuffer)
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

    bool VertexBuffer_RenderAPIObject_Vulkan::render(const RenderOptions* renderOptions)
    {
        if (renderOptions == nullptr)
        {
            JUMA_LOG(error, JSTR("Invalid input params"));
            return false;
        }

        const RenderOptions_Vulkan* options = reinterpret_cast<const RenderOptions_Vulkan*>(renderOptions);
        draw(options->commandBuffer->get());
        return true;
    }
}

#endif
