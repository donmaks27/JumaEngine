// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>
#include "subsystems/render/Vulkan/vma_lib.h"

#include "VulkanQueueType.h"
#include "jutils/jset.h"
#include "jutils/jshared_ptr.h"

namespace JumaEngine
{
    class VulkanBuffer : public VulkanContextObject
    {
    public:
        VulkanBuffer() = default;
        virtual ~VulkanBuffer() override;

        bool init(uint64 size, VkBufferUsageFlags usage, const jset<VulkanQueueType>& queues, VmaMemoryUsage memoryUsage, 
            VkMemoryPropertyFlags memoryProperties);
        bool init(const VkBufferCreateInfo& bufferInfo, const VmaAllocationCreateInfo& allocationInfo);

        VkBuffer get() const { return m_Buffer; }
        uint64 getSize() const { return m_BufferSize; }

        bool setData(const void* data) { return isValid() && setData(data, m_BufferSize); }
        bool setData(const void* data, uint64 dataSize);

        bool copyTo(const jshared_ptr<VulkanBuffer>& dstBuffer) const;

        bool initGPUBuffer(const void* data, uint64 dataSize, const jset<VulkanQueueType>& queues, VkBufferUsageFlags usage);

    protected:

        virtual void clearInternal() override { clearBuffer(); }

    private:

        VkBuffer m_Buffer = nullptr;
        VmaAllocation m_Allocation = nullptr;
        uint64 m_BufferSize = 0;


        void clearBuffer();

        bool copyTo(const VulkanBuffer* dstBuffer) const;
    };
}

#endif
