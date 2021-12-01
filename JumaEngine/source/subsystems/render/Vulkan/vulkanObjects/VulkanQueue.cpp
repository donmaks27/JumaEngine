// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VulkanQueue.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "utils/jlog.h"

namespace JumaEngine
{
    VulkanQueue::~VulkanQueue()
    {
        if (isValid())
        {
            clearQueue();
        }
    }

    bool VulkanQueue::init(const uint32 familyIndex, const uint32 queueIndex)
    {
        if (isValid())
        {
            return false;
        }

        VkDevice device = getRenderSubsystem()->getDevice();
        if (device == nullptr)
        {
            return false;
        }
        vkGetDeviceQueue(device, familyIndex, queueIndex, &m_Queue);
        if (m_Queue == nullptr)
        {
            JUMA_LOG(warning, JSTR("Failed to get queue ") + TO_JSTR(queueIndex) + JSTR(" from family ") + TO_JSTR(familyIndex));
            return false;
        }

        m_QueueFamilyIndex = familyIndex;
        m_QueueIndex = queueIndex;
        markAsInitialized();
        return true;
    }

    void VulkanQueue::clearQueue()
    {
        m_QueueFamilyIndex = 0;
        m_QueueIndex = 0;
        m_Queue = nullptr;
    }
}

#endif
