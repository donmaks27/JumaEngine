// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VulkanCommandPool.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "VulkanCommandBuffer.h"

namespace JumaEngine
{
    VulkanCommandPool::~VulkanCommandPool()
    {
        clearVulkan();
    }

    bool VulkanCommandPool::init(const VulkanQueueType queueType, const VkCommandPoolCreateFlags flags)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Command pool already initialized"));
            return false;
        }

        const uint32 queueFamiyIndex = getRenderSubsystemObject()->getQueueFamilyIndex(queueType);
        VkCommandPoolCreateInfo commandPoolInfo{};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.queueFamilyIndex = queueFamiyIndex;
        commandPoolInfo.flags = flags | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        const VkResult result = vkCreateCommandPool(getRenderSubsystemObject()->getDevice(), &commandPoolInfo, nullptr, &m_CommandPool);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create command pull for queue family ") + TO_JSTR(queueFamiyIndex), result);
            return false;
        }

        m_QueueType = queueType;
        markAsInitialized();
        return true;
    }

    void VulkanCommandPool::clearVulkan()
    {
        m_UnusedCommandBuffers.clear();
        m_CommandBuffers.clear();
        if (m_CommandPool != nullptr)
        {
            vkDestroyCommandPool(getRenderSubsystemObject()->getDevice(), m_CommandPool, nullptr);
        }
    }

    VulkanCommandBuffer* VulkanCommandPool::getCommandBuffer()
    {
        if (!isValid())
        {
            return nullptr;
        }

        if (!m_UnusedCommandBuffers.isEmpty())
        {
            VulkanCommandBuffer* result = m_UnusedCommandBuffers.getLast();
            m_UnusedCommandBuffers.removeLast();
            return result;
        }

        VulkanCommandBuffer* result = &m_CommandBuffers.addDefault();
        if (!createCommandBuffers(true, 1, &result->m_CommandBuffer))
        {
            m_CommandBuffers.removeLast();
            return nullptr;
        }

        result->m_CommandPool = this;
        return result;
    }
    void VulkanCommandPool::returnCommandBuffer(VulkanCommandBuffer* commandBuffer)
    {
        if (isValid() && (commandBuffer != nullptr))
        {
            vkResetCommandBuffer(commandBuffer->get(), VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
            m_UnusedCommandBuffers.add(commandBuffer);
        }
    }

    bool VulkanCommandPool::createCommandBuffers(const bool primaryLevel, const int32 count, VkCommandBuffer* commandBuffers)
    {
        if (!isValid() || (count <= 0))
        {
            return false;
        }

        VkCommandBufferAllocateInfo commandBufferInfo{};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferInfo.commandPool = m_CommandPool;
        commandBufferInfo.level = primaryLevel ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        commandBufferInfo.commandBufferCount = static_cast<uint32>(count);
        const VkResult result = vkAllocateCommandBuffers(getRenderSubsystemObject()->getDevice(), &commandBufferInfo, commandBuffers);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to allocate command buffers"), result);
            return false;
        }
        return true;
    }
}

#endif
