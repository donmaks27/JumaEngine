// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/window/Window.h"

#include <vulkan/vulkan_core.h>

namespace JumaEngine
{
    class RenderSubsystem_Vulkan;
    class VulkanSwapchain;

    class Window_Vulkan : public Window
    {
        JUMAENGINE_ABSTRACT_CLASS(Window_Vulkan, Window)

    public:
        Window_Vulkan() = default;
        virtual ~Window_Vulkan() override = default;

        VkSurfaceKHR getVulkanSurface() const { return m_VulkanSurface; }

        bool pickSurfaceFormat(VkSurfaceFormatKHR& outSurfaceFormat) const;

        bool createVulkanSwapchain();
        VulkanSwapchain* getVulkanSwapchain() const { return m_VulkanSwapchain; }
        void destroyVulkanSwapchain();

    protected:

        VkSurfaceKHR m_VulkanSurface = nullptr;
        VulkanSwapchain* m_VulkanSwapchain = nullptr;


        RenderSubsystem_Vulkan* getRenderSubsystem() const;
        VkInstance getVulkanInstance() const;

        void destroyWindow_Vulkan();
    };
}

#endif
