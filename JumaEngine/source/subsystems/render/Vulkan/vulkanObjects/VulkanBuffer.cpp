// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VulkanBuffer.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include <glm/common.hpp>
#include "utils/jlog.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"

namespace JumaEngine
{
    VulkanBuffer::~VulkanBuffer()
    {
        clearBuffer();
    }

    bool VulkanBuffer::init(VkDeviceSize size, VkBufferUsageFlags usage, const jset<VulkanQueueType>& queues, VmaMemoryUsage memoryUsage,
        VkMemoryPropertyFlags memoryProperties)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Buffer already initialized."));
            return false;
        }

        jarray<uint32> uniqueQueueFamilies;
        for (const auto& queueType : queues)
        {
            uniqueQueueFamilies.addUnique(getRenderSubsystem()->getQueueFamilyIndex(queueType));
        }
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        if (uniqueQueueFamilies.getSize() > 1)
        {
            bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
            bufferInfo.queueFamilyIndexCount = static_cast<uint32>(uniqueQueueFamilies.getSize());
            bufferInfo.pQueueFamilyIndices = uniqueQueueFamilies.getData();
        }
        else
        {
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            bufferInfo.queueFamilyIndexCount = 0;
            bufferInfo.pQueueFamilyIndices = nullptr;
        }
        VmaAllocationCreateInfo allocationInfo{};
        allocationInfo.flags = 0;
        allocationInfo.usage = memoryUsage;
        allocationInfo.requiredFlags = memoryProperties;
        return init(bufferInfo, allocationInfo);
    }
    bool VulkanBuffer::init(const VkBufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocationInfo)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Buffer already initialized."));
            return false;
        }

        const VkResult result = vmaCreateBuffer(getRenderSubsystem()->getAllocator(), &bufferInfo, &allocationInfo, &m_Buffer, &m_Allocation, nullptr);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(error, JSTR("Failed to create buffer. Code ") + TO_JSTR(result));
            return false;
        }

        m_BufferSize = bufferInfo.size;
        markAsInitialized();
        return true;
    }

    void VulkanBuffer::clearBuffer()
    {
        vmaDestroyBuffer(getRenderSubsystem()->getAllocator(), m_Buffer, m_Allocation);

        m_Buffer = nullptr;
        m_Allocation = nullptr;
        m_BufferSize = 0;
    }

    bool VulkanBuffer::setData(const void* data, const uint64 dataSize)
    {
        if (!isValid() || (data == nullptr) || (dataSize == 0) || (dataSize > m_BufferSize))
        {
            return false;
        }

        void* mappedData;
        if (vmaMapMemory(getRenderSubsystem()->getAllocator(), m_Allocation, &mappedData) != VK_SUCCESS)
        {
            return false;
        }
        memcpy(mappedData, data, dataSize);
        vmaUnmapMemory(getRenderSubsystem()->getAllocator(), m_Allocation);
        return true;
    }

    bool VulkanBuffer::copyTo(const jshared_ptr<VulkanBuffer>& dstBuffer) const
    {
        if (!isValid() || (dstBuffer == nullptr) || !dstBuffer->isValid())
        {
            return false;
        }

        const jshared_ptr<VulkanCommandPool> commandPool = getRenderSubsystem()->getCommandPool(VulkanQueueType::Transfer);
        const jshared_ptr<VulkanCommandBuffer> commandBuffer = commandPool != nullptr ? commandPool->createCommandBuffer(true) : nullptr;
        if ((commandBuffer == nullptr) || !commandBuffer->isValid())
        {
            return false;
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer->get(), &beginInfo);

        VkBufferCopy copyRegion;
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = glm::min(m_BufferSize, dstBuffer->getSize());
        vkCmdCopyBuffer(commandBuffer->get(), m_Buffer, dstBuffer->get(), 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer->get());

        return commandBuffer->submit(true);
    }
}

#endif
