// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

namespace JumaEngine
{
    class RenderSubsystem_Vulkan;

    class VulkanContextObject
    {
        friend RenderSubsystem_Vulkan;

    public:
        VulkanContextObject() = default;
        virtual ~VulkanContextObject() = default;

        RenderSubsystem_Vulkan* getRenderSubsystem() const { return m_RenderSubsystem; }

        bool isValid() const { return m_Initialized; }
        void clear()
        {
            if (isValid())
            {
                clearInternal();
                m_Initialized = false;
            }
        }

    protected:

        void markAsInitialized() { m_Initialized = true; }

        virtual void clearInternal() = 0;

    private:

        RenderSubsystem_Vulkan* m_RenderSubsystem = nullptr;

        bool m_Initialized = false;
    };
}

#endif