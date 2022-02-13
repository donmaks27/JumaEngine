// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

namespace JumaEngine
{
    class RenderSubsystem_Vulkan;

    class VulkanContextObjectBase
    {
        friend RenderSubsystem_Vulkan;

    public:
        VulkanContextObjectBase() = default;
        virtual ~VulkanContextObjectBase() = default;

        RenderSubsystem_Vulkan* getRenderSubsystem() const { return m_RenderSubsystem; }

    private:

        RenderSubsystem_Vulkan* m_RenderSubsystem = nullptr;
    };

    class VulkanContextObject : public VulkanContextObjectBase
    {
    public:
        VulkanContextObject() = default;
        virtual ~VulkanContextObject() override = default;

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

        bool m_Initialized = false;
    };
}

#define JUMA_VULKAN_ERROR_LOG(message, code) JUMA_LOG(error, jstring(message) + JSTR(". Code ") + TO_JSTR(code))

#endif