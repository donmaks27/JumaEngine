// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VulkanCommandBuffer.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

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
}

#endif
