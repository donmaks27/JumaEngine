// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "WindowSubsystem_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "engine/Engine.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "subsystems/render/Vulkan/vulkanObjects/VulkanSwapchain.h"

namespace JumaEngine
{
    RenderSubsystem_RenderAPIObject_Vulkan* WindowSubsystem_RenderAPIObject_Vulkan::getRenderSubsystem() const
    {
        return m_Parent->getOwnerEngine()->getRenderSubsystem()->getRenderAPIObject<RenderSubsystem_RenderAPIObject_Vulkan>();
    }

    void WindowSubsystem_RenderAPIObject_Vulkan::destroyWindow_Vulkan(const window_id_type windowID, WindowDescription_Vulkan& description)
    {
        destroyVulkanSwapchain(windowID, description);

        if (description.vulkanSurface != nullptr)
        {
            vkDestroySurfaceKHR(getRenderSubsystem()->getVulkanInstance(), description.vulkanSurface, nullptr);
            description.vulkanSurface = nullptr;
        }
    }
    
    VkSurfaceKHR WindowSubsystem_RenderAPIObject_Vulkan::getWindowVulkanSurface(const window_id_type windowID) const
    {
        const WindowDescription_Vulkan* description = findWindow<WindowDescription_Vulkan>(windowID);
        return description != nullptr ? description->vulkanSurface : nullptr;
    }
    void WindowSubsystem_RenderAPIObject_Vulkan::updateSupportedPresentModes(const window_id_type windowID)
    {
        WindowDescription_Vulkan* description = findWindow<WindowDescription_Vulkan>(windowID);
        WindowDescription* parentDescription = findParentWindow(windowID);
        if ((description != nullptr) && (parentDescription != nullptr))
        {
            updateSupportedPresentModes(windowID, *description, *parentDescription);
        }
    }
    void WindowSubsystem_RenderAPIObject_Vulkan::updateSupportedPresentModes(const window_id_type windowID, WindowDescription_Vulkan& description, 
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

    bool WindowSubsystem_RenderAPIObject_Vulkan::createVulkanSwapchain(const window_id_type windowID)
    {
        const WindowDescription* parentDescription = findParentWindow(windowID);
        WindowDescription_Vulkan* description = findWindow<WindowDescription_Vulkan>(windowID);
        if ((parentDescription == nullptr) || (description == nullptr))
        {
            JUMA_LOG(error, JSTR("Can't find window"));
            return false;
        }
        return createVulkanSwapchain(windowID, *description, parentDescription->size);
    }
    bool WindowSubsystem_RenderAPIObject_Vulkan::createVulkanSwapchain(const window_id_type windowID, WindowDescription_Vulkan& description, 
        const math::uvector2& size)
    {
        if (getRenderSubsystem()->getPhysicalDevice() == nullptr)
        {
            return false;
        }

        if (description.vulkanSwapchain != nullptr)
        {
            JUMA_LOG(warning, JSTR("Window's swapchain already created"));
            return false;
        }

        VkSurfaceFormatKHR surfaceFormat;
        if (!pickSurfaceFormat(description.vulkanSurface, surfaceFormat))
        {
            JUMA_LOG(error, JSTR("Failed to pick surface format"));
            return false;
        }

        VulkanSwapchain* swapchain = getRenderSubsystem()->createVulkanObject<VulkanSwapchain>();
        if ((swapchain == nullptr) || !swapchain->init(description.vulkanSurface, surfaceFormat, size))
        {
            JUMA_LOG(error, JSTR("Failed to create swapchain"));
            delete swapchain;
            return false;
        }

        description.vulkanSwapchain = swapchain;
        return true;
    }
    VulkanSwapchain* WindowSubsystem_RenderAPIObject_Vulkan::getVulkanSwapchain(const window_id_type windowID) const
    {
        const WindowDescription_Vulkan* description = findWindow<WindowDescription_Vulkan>(windowID);
        return description != nullptr ? description->vulkanSwapchain : nullptr;
    }
    void WindowSubsystem_RenderAPIObject_Vulkan::destroyVulkanSwapchain(const window_id_type windowID)
    {
        WindowDescription_Vulkan* description = findWindow<WindowDescription_Vulkan>(windowID);
        if (description != nullptr)
        {
            destroyVulkanSwapchain(windowID, *description);
        }
    }
    void WindowSubsystem_RenderAPIObject_Vulkan::destroyVulkanSwapchain(const window_id_type windowID, WindowDescription_Vulkan& description)
    {
        if (description.vulkanSwapchain != nullptr)
        {
            delete description.vulkanSwapchain;
            description.vulkanSwapchain = nullptr;
        }
    }
    
    bool WindowSubsystem_RenderAPIObject_Vulkan::pickSurfaceFormat(VkSurfaceKHR surface, VkSurfaceFormatKHR& outSurfaceFormat) const
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

    void WindowSubsystem_RenderAPIObject_Vulkan::onWindowResized(window_id_type windowID, const math::uvector2& newSize)
    {
        Super::onWindowResized(windowID, newSize);

        VulkanSwapchain* swapchain = getVulkanSwapchain(windowID);
        if (swapchain != nullptr)
        {
            swapchain->onWindowSizeChanged(newSize);
        }
    }

    void WindowSubsystem_RenderAPIObject_Vulkan::onFinishRender()
    {
        Super::onFinishRender();

        for (const auto& window : getParentWindows())
        {
            VulkanSwapchain* swapchain = getVulkanSwapchain(window.key);
            if ((swapchain != nullptr) && swapchain->isNeedToRecreate())
            {
                swapchain->applySettings(true);
            }
        }
    }
}

#endif
