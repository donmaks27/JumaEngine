// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include <vulkan/vulkan_core.h>

namespace JumaEngine
{
    class VulkanCommandPool;

    class VulkanCommandBuffer
    {
        friend VulkanCommandPool;

    public:
        VulkanCommandBuffer() = default;
        ~VulkanCommandBuffer() = default;

        bool isValid() const { return m_CommandBuffer != nullptr; }

        VkCommandBuffer get() const { return m_CommandBuffer; }

        bool submit(bool waitForFinish);
        bool submit(VkSubmitInfo submitInfo, VkFence fenceOnFinish, bool waitForFinish);

        void returnToCommandPool();

    private:

        VulkanCommandPool* m_CommandPool = nullptr;
        VkCommandBuffer m_CommandBuffer = nullptr;
    };
}

#endif
