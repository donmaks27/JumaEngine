// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VulkanCommandPool.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "jutils/jlog.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "VulkanCommandBuffer.h"

namespace JumaEngine
{
    VulkanCommandPool::~VulkanCommandPool()
    {
        if (isValid())
        {
            clearCommandPool();
        }
    }

    bool VulkanCommandPool::init(const VulkanQueueType queueType, const VkCommandPoolCreateFlags flags)
    {
        if (isValid())
        {
            return false;
        }

        const uint32 queueFamiyIndex = getRenderSubsystem()->getQueueFamilyIndex(queueType);
        VkCommandPoolCreateInfo commandPoolInfo{};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.queueFamilyIndex = queueFamiyIndex;
        commandPoolInfo.flags = flags;
        const VkResult result = vkCreateCommandPool(getRenderSubsystem()->getDevice(), &commandPoolInfo, nullptr, &m_CommandPool);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(warning, JSTR("Failed to create command pull for queue family ") + TO_JSTR(queueFamiyIndex) + JSTR(". Code ") + TO_JSTR(result));
            return false;
        }

        m_QueueType = queueType;
        markAsInitialized();
        return true;
    }

    void VulkanCommandPool::clearCommandPool()
    {
        clearCommandBuffers(m_CreatedCommandBuffers);
        m_CreatedCommandBuffers.clear();

        vkDestroyCommandPool(getRenderSubsystem()->getDevice(), m_CommandPool, nullptr);
        m_CommandPool = nullptr;
    }

    jarray<jshared_ptr<VulkanCommandBuffer>> VulkanCommandPool::createCommandBuffers(const bool primaryLevel, const uint32_t buffersCount)
    {
        if (!isValid() || (buffersCount == 0))
        {
            return {};
        }

        jarray<VkCommandBuffer> vulkanCommandBuffers(buffersCount);
        VkCommandBufferAllocateInfo commandBufferInfo{};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = m_CommandPool;
        commandBufferInfo.level = primaryLevel ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        commandBufferInfo.commandBufferCount = buffersCount;
        const VkResult result = vkAllocateCommandBuffers(getRenderSubsystem()->getDevice(), &commandBufferInfo, vulkanCommandBuffers.getData());
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(warning, JSTR("Failed to create command buffers. Code ") + TO_JSTR(result));
            return {};
        }

        jarray<jshared_ptr<VulkanCommandBuffer>> commandBuffers;
        for (const auto& buffer : vulkanCommandBuffers)
        {
            commandBuffers.add(createCommandBuffer(buffer));
        }
        return commandBuffers;
    }
    jshared_ptr<VulkanCommandBuffer> VulkanCommandPool::createCommandBuffer(const bool primaryLevel)
    {
        if (!isValid())
        {
            return nullptr;
        }

        VkCommandBuffer vulkanCommandBuffer = nullptr;
        VkCommandBufferAllocateInfo commandBufferInfo{};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = m_CommandPool;
        commandBufferInfo.level = primaryLevel ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        commandBufferInfo.commandBufferCount = 1;
        const VkResult result = vkAllocateCommandBuffers(getRenderSubsystem()->getDevice(), &commandBufferInfo, &vulkanCommandBuffer);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(warning, JSTR("Failed to create command buffer. Code ") + TO_JSTR(result));
            return nullptr;
        }
        return createCommandBuffer(vulkanCommandBuffer);
    }
    jshared_ptr<VulkanCommandBuffer> VulkanCommandPool::createCommandBuffer(VkCommandBuffer buffer)
    {
        jshared_ptr<VulkanCommandBuffer> commandBuffer = getRenderSubsystem()->createVulkanObject<VulkanCommandBuffer>();
        m_CreatedCommandBuffers.add(commandBuffer.get());
        commandBuffer->m_ParentCommandPool = this;
        commandBuffer->m_CommandBuffer = buffer;
        commandBuffer->onPreClear.bind(this, &VulkanCommandPool::onCommandBufferPreClear);
        commandBuffer->markAsInitialized();
        return commandBuffer;
    }

    void VulkanCommandPool::clearCommandBuffers(const jarray<jshared_ptr<VulkanCommandBuffer>>& commandBuffers)
    {
        if (!isValid() || commandBuffers.isEmpty())
        {
            return;
        }

        jarray<VulkanCommandBuffer*> buffers;
        for (const auto& commandBuffer : commandBuffers)
        {
            m_CreatedCommandBuffers.remove(commandBuffer.get());
            buffers.add(commandBuffer.get());
        }
        clearCommandBuffers(buffers);
    }
    void VulkanCommandPool::clearCommandBuffers(const jarray<VulkanCommandBuffer*>& commandBuffers)
    {
        if (!isValid() || commandBuffers.isEmpty())
        {
            return;
        }

        jarray<VkCommandBuffer> vulkanCommandBuffers;
        for (const auto& commandBuffer : commandBuffers)
        {
            if ((commandBuffer != nullptr) && commandBuffer->isValid())
            {
                vulkanCommandBuffers.add(commandBuffer->get());
                commandBuffer->onPreClear.unbind(this, &VulkanCommandPool::onCommandBufferPreClear);
                commandBuffer->clear();
            }
        }
        if (!vulkanCommandBuffers.isEmpty())
        {
            vkFreeCommandBuffers(getRenderSubsystem()->getDevice(), m_CommandPool, static_cast<uint32_t>(vulkanCommandBuffers.getSize()), vulkanCommandBuffers.getData());
        }
    }
    void VulkanCommandPool::onCommandBufferPreClear(VulkanCommandBuffer* commandBuffer)
    {
        commandBuffer->onPreClear.unbind(this, &VulkanCommandPool::onCommandBufferPreClear);
        m_CreatedCommandBuffers.remove(commandBuffer);

        VkCommandBuffer buffer = commandBuffer->get();
        vkFreeCommandBuffers(getRenderSubsystem()->getDevice(), m_CommandPool, 1, &buffer);
    }
}

#endif
