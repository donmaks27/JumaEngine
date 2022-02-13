// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Window_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "engine/Engine.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "subsystems/render/Vulkan/vulkanObjects/VulkanSwapchain.h"

namespace JumaEngine
{
    RenderSubsystem_Vulkan* Window_Vulkan::getRenderSubsystem() const
    {
        return cast<RenderSubsystem_Vulkan>(getOwnerEngine()->getRenderSubsystem());
    }
    VkInstance Window_Vulkan::getVulkanInstance() const
    {
        return getRenderSubsystem()->getVulkanInstance();
    }

    void Window_Vulkan::destroyWindow_Vulkan()
    {
        destroyVulkanSwapchain();

        if (m_VulkanSurface != nullptr)
        {
            vkDestroySurfaceKHR(getVulkanInstance(), m_VulkanSurface, nullptr);
            m_VulkanSurface = nullptr;
        }
    }

    bool Window_Vulkan::pickSurfaceFormat(VkSurfaceFormatKHR& outSurfaceFormat) const
    {
        if (m_VulkanSurface == nullptr)
        {
            return false;
        }

        RenderSubsystem_Vulkan* renderSubsystem = getRenderSubsystem();
        uint32 surfaceFormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(renderSubsystem->getPhysicalDevice(), m_VulkanSurface, &surfaceFormatCount, nullptr);
        if (surfaceFormatCount == 0)
        {
            return false;
        }

        jarray<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(renderSubsystem->getPhysicalDevice(), m_VulkanSurface, &surfaceFormatCount, surfaceFormats.getData());

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

    bool Window_Vulkan::createVulkanSwapchain()
    {
        if (!isValid())
        {
            JUMA_LOG(warning, JSTR("Window not initialized"));
            return false;
        }
        if (m_VulkanSwapchain != nullptr)
        {
            JUMA_LOG(warning, JSTR("Swapchain already initialized"));
            return false;
        }

        VulkanSwapchain* swapchain = getRenderSubsystem()->createVulkanObject<VulkanSwapchain>();
        if ((swapchain == nullptr) || !swapchain->init(this))
        {
            JUMA_LOG(error, JSTR("Failed to create swapchain"));
            delete swapchain;
            return false;
        }

        m_VulkanSwapchain = swapchain;
        return true;
    }
    void Window_Vulkan::destroyVulkanSwapchain()
    {
        if (m_VulkanSwapchain != nullptr)
        {
            delete m_VulkanSwapchain;
            m_VulkanSwapchain = nullptr;
        }
    }
}

#endif
