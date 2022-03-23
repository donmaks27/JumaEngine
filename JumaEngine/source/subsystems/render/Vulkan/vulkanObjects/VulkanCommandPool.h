// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "VulkanQueueType.h"
#include "jutils/jlist.h"
#include "jutils/jarray.h"
#include "VulkanCommandBuffer.h"

namespace JumaEngine
{
    class VulkanCommandPool : public VulkanContextObject
    {
    public:
        VulkanCommandPool() = default;
        virtual ~VulkanCommandPool() override;

        bool init(VulkanQueueType queueType, VkCommandPoolCreateFlags flags = 0);

        VkCommandPool get() const { return m_CommandPool; }
        VulkanQueueType getQueueType() const { return m_QueueType; }

        VulkanCommandBuffer* getCommandBuffer();
        void returnCommandBuffer(VulkanCommandBuffer* commandBuffer);

    protected:

        virtual void clearInternal() override { clearVulkan(); }

    private:

        VkCommandPool m_CommandPool = nullptr;
        VulkanQueueType m_QueueType = VulkanQueueType::Graphics;

        jlist<VulkanCommandBuffer> m_CommandBuffers;
        jarray<VulkanCommandBuffer*> m_UnusedCommandBuffers;


        void clearVulkan();

        bool createCommandBuffers(bool primaryLevel, int32 count, VkCommandBuffer* commandBuffers);
    };
}

#endif
