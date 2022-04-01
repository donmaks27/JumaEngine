﻿// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "WindowSubsystem_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "engine/Engine.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "subsystems/render/Vulkan/vulkanObjects/VulkanSwapchain.h"
#include "subsystems/render/Vulkan/vulkanObjects/VulkanRenderImage.h"

namespace JumaEngine
{
    RenderSubsystem_Vulkan* WindowSubsystemRenderAPIObject_Vulkan::getRenderSubsystem() const
    {
        return dynamic_cast<RenderSubsystem_Vulkan*>(m_Parent->getOwnerEngine()->getRenderSubsystem());
    }

    void WindowSubsystemRenderAPIObject_Vulkan::destroyWindow_Vulkan(const window_id_type windowID, WindowDescription_Vulkan& description)
    {
        destroyRenderImage(windowID, description);
        destroyVulkanSwapchain(windowID, description);

        if (description.vulkanSurface != nullptr)
        {
            vkDestroySurfaceKHR(getRenderSubsystem()->getVulkanInstance(), description.vulkanSurface, nullptr);
            description.vulkanSurface = nullptr;
        }
    }
    
    VkSurfaceKHR WindowSubsystemRenderAPIObject_Vulkan::getWindowVulkanSurface(const window_id_type windowID) const
    {
        const WindowDescription_Vulkan* description = findWindow<WindowDescription_Vulkan>(windowID);
        return description != nullptr ? description->vulkanSurface : nullptr;
    }
    void WindowSubsystemRenderAPIObject_Vulkan::updateSupportedPresentModes(const window_id_type windowID)
    {
        WindowDescription_Vulkan* description = findWindow<WindowDescription_Vulkan>(windowID);
        WindowDescription* parentDescription = findParentWindow(windowID);
        if ((description != nullptr) && (parentDescription != nullptr))
        {
            updateSupportedPresentModes(windowID, *description, *parentDescription);
        }
    }
    void WindowSubsystemRenderAPIObject_Vulkan::updateSupportedPresentModes(const window_id_type windowID, WindowDescription_Vulkan& description, 
        WindowDescription& parentDescription)
    {
        VkPhysicalDevice physicalDevice = getRenderSubsystem()->getPhysicalDevice();
        if (physicalDevice == nullptr)
        {
            return;
        }

        uint32 surfacePresentModeCount;
	    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, description.vulkanSurface, &surfacePresentModeCount, nullptr);
	    jarray<VkPresentModeKHR> surfacePresentModes(static_cast<int32>(surfacePresentModeCount));
	    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, description.vulkanSurface, &surfacePresentModeCount, surfacePresentModes.getData());

        parentDescription.supportedPresentModes = { RenderPresentMode::VSYNC };
        if (surfacePresentModes.contains(VK_PRESENT_MODE_IMMEDIATE_KHR))
        {
            parentDescription.supportedPresentModes.add(RenderPresentMode::IMMEDIATE);
        }
        if (surfacePresentModes.contains(VK_PRESENT_MODE_MAILBOX_KHR))
        {
            parentDescription.supportedPresentModes.add(RenderPresentMode::TRIPLE_BUFFER);
        }
    }

    bool WindowSubsystemRenderAPIObject_Vulkan::createVulkanSwapchain(const window_id_type windowID)
    {
        const WindowDescription* parentDescription = findParentWindow(windowID);
        WindowDescription_Vulkan* description = findWindow<WindowDescription_Vulkan>(windowID);
        if ((parentDescription == nullptr) || (description == nullptr))
        {
            JUMA_LOG(error, JSTR("Can't find window"));
            return false;
        }
        if (description->vulkanSwapchain != nullptr)
        {
            JUMA_LOG(warning, JSTR("Window's swapchain already created"));
            return false;
        }

        VkSurfaceFormatKHR surfaceFormat;
        if (!pickSurfaceFormat(description->vulkanSurface, surfaceFormat))
        {
            JUMA_LOG(error, JSTR("Failed to pick surface format"));
            return false;
        }

        VulkanSwapchain* swapchain = getRenderSubsystem()->createVulkanObject<VulkanSwapchain>();
        if ((swapchain == nullptr) || !swapchain->init(description->vulkanSurface, surfaceFormat, parentDescription->size))
        {
            JUMA_LOG(error, JSTR("Failed to create swapchain"));
            delete swapchain;
            return false;
        }

        description->vulkanSwapchain = swapchain;
        return true;
    }
    VulkanSwapchain* WindowSubsystemRenderAPIObject_Vulkan::getVulkanSwapchain(const window_id_type windowID) const
    {
        const WindowDescription_Vulkan* description = findWindow<WindowDescription_Vulkan>(windowID);
        return description != nullptr ? description->vulkanSwapchain : nullptr;
    }
    void WindowSubsystemRenderAPIObject_Vulkan::destroyVulkanSwapchain(const window_id_type windowID)
    {
        WindowDescription_Vulkan* description = findWindow<WindowDescription_Vulkan>(windowID);
        if (description != nullptr)
        {
            destroyVulkanSwapchain(windowID, *description);
        }
    }
    void WindowSubsystemRenderAPIObject_Vulkan::destroyVulkanSwapchain(const window_id_type windowID, WindowDescription_Vulkan& description)
    {
        if (description.vulkanSwapchain != nullptr)
        {
            delete description.vulkanSwapchain;
            description.vulkanSwapchain = nullptr;
        }
    }
    
    bool WindowSubsystemRenderAPIObject_Vulkan::pickSurfaceFormat(VkSurfaceKHR surface, VkSurfaceFormatKHR& outSurfaceFormat) const
    {
        if (surface == nullptr)
        {
            JUMA_LOG(error, JSTR("Can't find window surface"));
            return false;
        }

        VkPhysicalDevice physicalDevice = getRenderSubsystem()->getPhysicalDevice();

        uint32 surfaceFormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
        if (surfaceFormatCount == 0)
        {
            JUMA_LOG(error, JSTR("There is no surface formats for this surface"));
            return false;
        }
        jarray<VkSurfaceFormatKHR> surfaceFormats(static_cast<int32>(surfaceFormatCount));
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.getData());

        for (const auto& surfaceFormat : surfaceFormats)
        {
            if ((surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB) && (surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
            {
                outSurfaceFormat = surfaceFormat;
                return true;
            }
        }
        outSurfaceFormat = surfaceFormats[0];
        return true;
    }

    bool WindowSubsystemRenderAPIObject_Vulkan::createRenderImage(const window_id_type windowID)
    {
        WindowDescription_Vulkan* description = findWindow<WindowDescription_Vulkan>(windowID);
        if (description == nullptr)
        {
            JUMA_LOG(error, JSTR("Can't find window"));
            return false;
        }
        if ((description->vulkanSwapchain == nullptr) || !description->vulkanSwapchain->isValid())
        {
            JUMA_LOG(error, JSTR("Swapchain not initialized"));
            return false;
        }

        VulkanRenderImage* renderImage = getRenderSubsystem()->createVulkanObject<VulkanRenderImage>();
        if (!renderImage->init(description->vulkanSwapchain) || !renderImage->update())
        {
            JUMA_LOG(error, JSTR("Failed to create window render image"));
            delete renderImage;
            return false;
        }

        description->renderImage = renderImage;
        return true;
    }
    VulkanRenderImage* WindowSubsystemRenderAPIObject_Vulkan::getRenderImage(const window_id_type windowID) const
    {
        const WindowDescription_Vulkan* description = findWindow<WindowDescription_Vulkan>(windowID);
        return description != nullptr ? description->renderImage : nullptr;
    }
    void WindowSubsystemRenderAPIObject_Vulkan::destroyRenderImage(const window_id_type windowID)
    {
        WindowDescription_Vulkan* description = findWindow<WindowDescription_Vulkan>(windowID);
        if (description == nullptr)
        {
            JUMA_LOG(warning, JSTR("Can't find window"));
            return;
        }
        destroyRenderImage(windowID, *description);
    }
    void WindowSubsystemRenderAPIObject_Vulkan::destroyRenderImage(const window_id_type windowID, WindowDescription_Vulkan& description)
    {
        if (description.renderImage != nullptr)
        {
            delete description.renderImage;
            description.renderImage = nullptr;
        }
    }

    void WindowSubsystemRenderAPIObject_Vulkan::onWindowResized(window_id_type windowID, const math::uvector2& newSize)
    {
        Super::onWindowResized(windowID, newSize);

        VulkanSwapchain* swapchain = getVulkanSwapchain(windowID);
        if (swapchain != nullptr)
        {
            swapchain->onWindowSizeChanged(newSize);
        }
    }
}

#endif
