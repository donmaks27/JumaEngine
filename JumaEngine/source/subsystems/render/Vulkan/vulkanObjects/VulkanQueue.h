// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

namespace JumaEngine
{
    class VulkanQueue : public VulkanContextObject
    {
    public:
        VulkanQueue() = default;
        virtual ~VulkanQueue() override;

        bool init(uint32 familyIndex, uint32 queueIndex = 0);

        uint32 getFamilyIndex() const { return m_QueueFamilyIndex; }
        uint32 getIndex() const { return m_QueueIndex; }
        VkQueue get() const { return m_Queue; }

    protected:

        virtual void clearInternal() override { clearQueue(); }

    private:

        uint32 m_QueueFamilyIndex = 0;
        uint32 m_QueueIndex = 0;
        VkQueue m_Queue = nullptr;


        void clearQueue();
    };
}

#endif
