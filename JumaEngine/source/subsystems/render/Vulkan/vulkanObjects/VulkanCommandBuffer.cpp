// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VulkanCommandBuffer.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "VulkanQueue.h"
#include "jutils/jshared_ptr.h"
#include "VulkanCommandPool.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"

namespace JumaEngine
{
    bool VulkanCommandBuffer::submit(const bool waitForFinish)
    {
        if (!isValid())
        {
            return false;
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        return submit(submitInfo, nullptr, waitForFinish);
    }
    bool VulkanCommandBuffer::submit(VkSubmitInfo submitInfo, VkFence fenceOnFinish, const bool waitForFinish)
    {
        if (!isValid())
        {
            return false;
        }

        const jshared_ptr<VulkanQueue> queue = m_CommandPool->getRenderSubsystem()->getQueue(m_CommandPool->getQueueType());
        if ((queue == nullptr) || !queue->isValid())
        {
            return false;
        }
        
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffer;
        const VkResult result = vkQueueSubmit(queue->get(), 1, &submitInfo, fenceOnFinish);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to submit command buffer"), result);
            return false;
        }

        if (waitForFinish)
        {
            vkQueueWaitIdle(queue->get());
        }
        return true;
    }

    void VulkanCommandBuffer::returnToCommandPool()
    {
        if (isValid())
        {
            m_CommandPool->returnCommandBuffer(this);
        }
    }
}

#endif
