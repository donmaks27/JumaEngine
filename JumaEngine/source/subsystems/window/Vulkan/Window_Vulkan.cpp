// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Window_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "subsystems/render/Vulkan/vulkanObjects/VulkanSwapchain.h"
#include "subsystems/render/Vulkan/vulkanObjects/VulkanRenderImage.h"

namespace JumaEngine
{
    void Window_Vulkan::destroyWindow_Vulkan()
    {
        destroyRenderImage();
        destroyVulkanSwapchain();

        if (m_VulkanSurface != nullptr)
        {
            vkDestroySurfaceKHR(getRenderSubsystem()->getVulkanInstance(), m_VulkanSurface, nullptr);
            m_VulkanSurface = nullptr;
        }
    }

    bool Window_Vulkan::pickSurfaceFormat(VkSurfaceFormatKHR& outSurfaceFormat) const
    {
        if (m_VulkanSurface == nullptr)
        {
            return false;
        }

        VkPhysicalDevice physicalDevice = getRenderSubsystem()->getPhysicalDevice();

        uint32 surfaceFormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_VulkanSurface, &surfaceFormatCount, nullptr);
        if (surfaceFormatCount == 0)
        {
            return false;
        }
        jarray<VkSurfaceFormatKHR> surfaceFormats(static_cast<int32>(surfaceFormatCount));
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_VulkanSurface, &surfaceFormatCount, surfaceFormats.getData());

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
    
    void Window_Vulkan::fillSupportedPresentModes()
    {
        if (m_VulkanSurface == nullptr)
        {
            return;
        }

        m_SupportedPresentModes = { RenderPresentMode::VSYNC };

        VkPhysicalDevice physicalDevice = getRenderSubsystem()->getPhysicalDevice();
        uint32 surfacePresentModeCount;
	    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_VulkanSurface, &surfacePresentModeCount, nullptr);
	    jarray<VkPresentModeKHR> surfacePresentModes(static_cast<int32>(surfacePresentModeCount));
	    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_VulkanSurface, &surfacePresentModeCount, surfacePresentModes.getData());

        if (surfacePresentModes.contains(VK_PRESENT_MODE_IMMEDIATE_KHR))
        {
            m_SupportedPresentModes.add(RenderPresentMode::IMMEDIATE);
        }
        if (surfacePresentModes.contains(VK_PRESENT_MODE_MAILBOX_KHR))
        {
            m_SupportedPresentModes.add(RenderPresentMode::TRIPLE_BUFFER);
        }
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

    bool Window_Vulkan::createRenderImage()
    {
        if (!isValid())
        {
            JUMA_LOG(warning, JSTR("Window not initialized"));
            return false;
        }
        if ((m_VulkanSwapchain == nullptr) || !m_VulkanSwapchain->isValid())
        {
            JUMA_LOG(warning, JSTR("Swapchain not initialized"));
            return false;
        }

        VulkanRenderImage* renderImage = getRenderSubsystem()->createVulkanObject<VulkanRenderImage>();
        if (!renderImage->init(m_VulkanSwapchain) || !renderImage->update())
        {
            JUMA_LOG(error, JSTR("Failed to create window render image"));
            delete renderImage;
            return false;
        }

        m_RenderImage = renderImage;
        return true;
    }
    void Window_Vulkan::destroyRenderImage()
    {
        if (m_RenderImage != nullptr)
        {
            delete m_RenderImage;
            m_RenderImage = nullptr;
        }
    }

    void Window_Vulkan::onWindowResized(const math::uvector2& newSize)
    {
        Super::onWindowResized(newSize);

        m_VulkanSwapchain->onWindowSizeChanged();
    }
}

#endif
