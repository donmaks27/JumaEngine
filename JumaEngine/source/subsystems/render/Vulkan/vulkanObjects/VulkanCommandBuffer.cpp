// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VulkanCommandBuffer.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "VulkanCommandPool.h"
#include "VulkanQueue.h"
#include "utils/jlog.h"
#include "utils/jshared_ptr.h"

namespace JumaEngine
{
    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        if (isValid())
        {
            clearCommandBuffer();
        }
    }

    void VulkanCommandBuffer::clearCommandBuffer()
    {
        onPreClear.call(this);

        m_ParentCommandPool = nullptr;
        m_CommandBuffer = nullptr;
    }

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

        const jshared_ptr<VulkanQueue> queue = getRenderSubsystem()->getQueue(m_ParentCommandPool->getQueueType());
        if ((queue == nullptr) || !queue->isValid())
        {
            return false;
        }
        
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffer;
        const VkResult result = vkQueueSubmit(queue->get(), 1, &submitInfo, fenceOnFinish);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(error, JSTR("Failed to submit command buffer. Code ") + TO_JSTR(result));
            return false;
        }

        if (waitForFinish)
        {
            vkQueueWaitIdle(queue->get());
        }
        return true;
    }
}

#endif
