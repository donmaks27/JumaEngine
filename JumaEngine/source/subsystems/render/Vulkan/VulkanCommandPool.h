// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "VulkanQueueType.h"
#include "utils/jshared_ptr.h"
#include "utils/jarray.h"

namespace JumaEngine
{
    class VulkanCommandBuffer;

    class VulkanCommandPool : public VulkanContextObject
    {
    public:
        VulkanCommandPool() = default;
        virtual ~VulkanCommandPool() override;

        bool init(VulkanQueueType queueType, VkCommandPoolCreateFlags flags = 0);

        VulkanQueueType getQueueType() const { return m_QueueType; }
        VkCommandPool get() const { return m_CommandPool; }

        jarray<jshared_ptr<VulkanCommandBuffer>> createCommandBuffers(bool primaryLevel, uint32_t buffersCount);
        jshared_ptr<VulkanCommandBuffer> createCommandBuffer(bool primaryLevel);

        void clearCommandBuffers(const jarray<jshared_ptr<VulkanCommandBuffer>>& commandBuffers);

    protected:

        virtual void clearInternal() override { clearCommandPool(); }

    private:

        VulkanQueueType m_QueueType = VulkanQueueType::Graphics;
        VkCommandPool m_CommandPool = nullptr;

        jarray<VulkanCommandBuffer*> m_CreatedCommandBuffers;


        jshared_ptr<VulkanCommandBuffer> createCommandBuffer(VkCommandBuffer buffer);

        void clearCommandPool();
        void clearCommandBuffers(const jarray<VulkanCommandBuffer*>& commandBuffers);
        void onCommandBufferPreClear(VulkanCommandBuffer* commandBuffer);
    };
}

#endif
