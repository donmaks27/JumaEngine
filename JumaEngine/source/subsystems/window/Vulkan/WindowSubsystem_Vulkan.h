// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/window/WindowSubsystem.h"

#include <vulkan/vulkan_core.h>

#include "jutils/jarray.h"

namespace JumaEngine
{
    class RenderSubsystem_RenderAPIObject_Vulkan;
    class VulkanRenderImage;
    class VulkanSwapchain;

    struct WindowDescription_Vulkan : WindowDescription_RenderAPI
    {
        VkSurfaceKHR vulkanSurface = nullptr;

        VulkanSwapchain* vulkanSwapchain = nullptr;
        VulkanRenderImage* renderImage = nullptr;
    };

    class WindowSubsystem_RenderAPIObject_Vulkan : public WindowSubsystem_RenderAPIObject
    {
        using Super = WindowSubsystem_RenderAPIObject;

    public:
        WindowSubsystem_RenderAPIObject_Vulkan() = default;
        virtual ~WindowSubsystem_RenderAPIObject_Vulkan() override = default;

        virtual jarray<const char*> getVulkanInstanceExtensions() const = 0;

        VkSurfaceKHR getWindowVulkanSurface(window_id_type windowID) const;
        void updateSupportedPresentModes(window_id_type windowID);

        bool createVulkanSwapchain(window_id_type windowID);
        VulkanSwapchain* getVulkanSwapchain(window_id_type windowID) const;
        void destroyVulkanSwapchain(window_id_type windowID);

        bool createRenderImage(window_id_type windowID);
        VulkanRenderImage* getRenderImage(window_id_type windowID) const;
        void destroyRenderImage(window_id_type windowID);

    protected:

        RenderSubsystem_RenderAPIObject_Vulkan* getRenderSubsystem() const;

        void updateSupportedPresentModes(window_id_type windowID, WindowDescription_Vulkan& description, 
            WindowDescription& parentDescription);

        void destroyWindow_Vulkan(window_id_type windowID, WindowDescription_Vulkan& description);
        void destroyVulkanSwapchain(window_id_type windowID, WindowDescription_Vulkan& description);
        void destroyRenderImage(window_id_type windowID, WindowDescription_Vulkan& description);

        virtual void onWindowResized(window_id_type windowID, const math::uvector2& newSize) override;

    private:
        
        bool pickSurfaceFormat(VkSurfaceKHR surface, VkSurfaceFormatKHR& outSurfaceFormat) const;
    };
}

#endif
