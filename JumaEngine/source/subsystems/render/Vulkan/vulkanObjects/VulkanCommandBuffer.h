// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "utils/jdelegate_multicast.h"

namespace JumaEngine
{
    class VulkanCommandBuffer;
    class VulkanCommandPool;

    CREATE_JUTILS_MULTICAST_DELEGATE_OneParam(OnVulkanCommandBufferEvent, VulkanCommandBuffer*, buffer);

    class VulkanCommandBuffer : public VulkanContextObject
    {
        friend VulkanCommandPool;

    public:
        VulkanCommandBuffer() = default;
        virtual ~VulkanCommandBuffer() override;

        OnVulkanCommandBufferEvent onPreClear;


        VkCommandBuffer get() const { return m_CommandBuffer; }

        bool submit(bool waitForFinish);
        bool submit(VkSubmitInfo submitInfo, VkFence fenceOnFinish, bool waitForFinish);

    protected:

        virtual void clearInternal() override { clearCommandBuffer(); }

    private:

        VulkanCommandPool* m_ParentCommandPool = nullptr;
        VkCommandBuffer m_CommandBuffer = nullptr;


        void clearCommandBuffer();
    };
}

#endif
