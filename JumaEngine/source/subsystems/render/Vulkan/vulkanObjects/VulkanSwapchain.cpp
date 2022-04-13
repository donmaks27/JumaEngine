// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VulkanSwapchain.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "VulkanQueue.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"

namespace JumaEngine
{
    VulkanSwapchain::~VulkanSwapchain()
    {
        if (isValid())
        {
            clearVulkanObjects();
        }
    }

    bool VulkanSwapchain::init(VkSurfaceKHR surface, const VkSurfaceFormatKHR surfaceFormat, const math::uvector2& windowSize)
    {
        if (isValid())
        {
            JUMA_LOG(error, JSTR("Swapchain already initialized"));
            return false;
        }
        if (surface == nullptr)
        {
            JUMA_LOG(error, JSTR("Invalid surface parameter"));
            return false;
        }

        // Calculate swapchain size
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getRenderSubsystemObject()->getPhysicalDevice(), surface, &capabilities);
        const VkExtent2D swapchainSize = capabilities.currentExtent.width != UINT32_MAX ? capabilities.currentExtent : VkExtent2D{
		    math::clamp(windowSize.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		    math::clamp(windowSize.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
	    };

        m_WindowSurface = surface;
        m_SwapchainImagesFormat = surfaceFormat;
        m_CurrentSettings.size = { swapchainSize.width, swapchainSize.height };
        m_CurrentSettings.presentMode = getRenderSubsystemObject()->getParent()->getPresentMode();

        if (!updateSwapchain() || !updateSyncObjects())
        {
            clearVulkanObjects();
            m_WindowSurface = nullptr;
            return false;
        }

        m_SettingForApply = m_CurrentSettings;
        markAsInitialized();
        return true;
    }

    bool VulkanSwapchain::updateSwapchain()
    {
        VkDevice device = getRenderSubsystemObject()->getDevice();
        
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getRenderSubsystemObject()->getPhysicalDevice(), m_WindowSurface, &capabilities);
        uint32 imageCount = m_CurrentSettings.presentMode != RenderPresentMode::TRIPLE_BUFFER ? 3 : 2;
        if (capabilities.maxImageCount > 0)
        {
            imageCount = math::min(imageCount, capabilities.maxImageCount);
        }
        imageCount = math::max(capabilities.minImageCount, imageCount);

        const uint32 queueFamilyIndices[2] = {
            getRenderSubsystemObject()->getQueueFamilyIndex(VulkanQueueType::Graphics),
            getRenderSubsystemObject()->getQueueFamilyIndex(VulkanQueueType::Present)
        };

        VkSwapchainKHR oldSwapchain = m_Swapchain;
        VkSwapchainCreateInfoKHR swapchainInfo{};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	    swapchainInfo.surface = m_WindowSurface;
	    swapchainInfo.minImageCount = imageCount;
	    swapchainInfo.imageFormat = m_SwapchainImagesFormat.format;
	    swapchainInfo.imageColorSpace = m_SwapchainImagesFormat.colorSpace;
	    swapchainInfo.imageExtent = { m_CurrentSettings.size.x, m_CurrentSettings.size.y };
	    swapchainInfo.imageArrayLayers = 1;
	    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	    if (queueFamilyIndices[0] != queueFamilyIndices[1])
	    {
		    swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		    swapchainInfo.queueFamilyIndexCount = 2;
		    swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
	    }
	    else
	    {
		    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		    swapchainInfo.queueFamilyIndexCount = 0;
		    swapchainInfo.pQueueFamilyIndices = nullptr;
	    }
	    swapchainInfo.preTransform = capabilities.currentTransform;
	    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	    swapchainInfo.presentMode = GetVulkanPresentModeByRenderPresentMode(m_CurrentSettings.presentMode);
	    swapchainInfo.clipped = VK_TRUE;
	    swapchainInfo.oldSwapchain = oldSwapchain;
        const VkResult result = vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &m_Swapchain);
        if (oldSwapchain != nullptr)
        {
            vkDestroySwapchainKHR(device, oldSwapchain, nullptr);
        }
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create swapchain"), result);
            return false;
        }

        vkGetSwapchainImagesKHR(device, m_Swapchain, &imageCount, nullptr);
        m_SwapchainImages.resize(static_cast<int32>(imageCount));
        vkGetSwapchainImagesKHR(device, m_Swapchain, &imageCount, m_SwapchainImages.getData());
        return true;
    }
    bool VulkanSwapchain::updateSyncObjects()
    {
        if (m_RenderAvailableSemaphore == nullptr)
        {
            VkDevice device = getRenderSubsystemObject()->getDevice();
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            const VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_RenderAvailableSemaphore);
            if (result != VK_SUCCESS)
            {
                JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create semaphore"), result);
                return false;
            }
        }
        return true;
    }

    void VulkanSwapchain::clearInternal()
    {
        clearVulkanObjects();

        m_WindowSurface = nullptr;
    }
    void VulkanSwapchain::clearVulkanObjects()
    {
        VkDevice device = getRenderSubsystemObject()->getDevice();

        if (m_RenderAvailableSemaphore != nullptr)
        {
            vkDestroySemaphore(device, m_RenderAvailableSemaphore, nullptr);
            m_RenderAvailableSemaphore = nullptr;
        }

        m_SwapchainImages.clear();
        if (m_Swapchain != nullptr)
        {
            vkDestroySwapchainKHR(device, m_Swapchain, nullptr);
            m_Swapchain = nullptr;
        }
    }

    void VulkanSwapchain::onWindowSizeChanged(const math::uvector2& newSize)
    {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getRenderSubsystemObject()->getPhysicalDevice(), m_WindowSurface, &capabilities);
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            m_SettingForApply.size = { capabilities.currentExtent.width, capabilities.currentExtent.height };
        }
        else
        {
            m_SettingForApply.size = {
		        math::clamp(newSize.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		        math::clamp(newSize.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
	        };
        }

        markAsNeededToRecreate();
    }

    void VulkanSwapchain::markAsNeededToRecreate()
    {
        if (isValid())
        {
            m_NeedToRecreate = true;
        }
    }
    void VulkanSwapchain::applySettings(const bool forceRecreate)
    {
        if (!isValid())
        {
            return;
        }
        if ((m_SettingForApply.size.x == 0) || (m_SettingForApply.size.y == 0))
        {
            markAsNeededToRecreate();
            return;
        }
        if (!applySettingsInternal(forceRecreate))
        {
            clearVulkanObjects();
        }
    }
    bool VulkanSwapchain::applySettingsInternal(const bool forceRecreate)
    {
        vkDeviceWaitIdle(getRenderSubsystemObject()->getDevice());
        m_NeedToRecreate = false;

        const bool sizeChanged = forceRecreate || (m_SettingForApply.size != m_CurrentSettings.size);
        const bool presentModeChanged = forceRecreate || (m_SettingForApply.presentMode != m_CurrentSettings.presentMode);

        const bool shouldRecreateSwapchain = sizeChanged || presentModeChanged;

        m_CurrentSettings = m_SettingForApply;
        if (shouldRecreateSwapchain && !updateSwapchain())
        {
            return false;
        }
        if (!updateSyncObjects())
        {
            return false;
        }
        return true;
    }

    bool VulkanSwapchain::acquireNextImage()
    {
        if (!isValid() || isNeedToRecreate())
        {
            return false;
        }

        uint32 renderImageIndex = 0;
        const VkResult result = vkAcquireNextImageKHR(getRenderSubsystemObject()->getDevice(), m_Swapchain, UINT64_MAX, m_RenderAvailableSemaphore, nullptr, &renderImageIndex);
        if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR))
        {
            if (result == VK_ERROR_OUT_OF_DATE_KHR)
            {
                markAsNeededToRecreate();
                return false;
            }

            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to acquire swapchain image"), result);
            throw std::runtime_error(JSTR("Failed to acquire swapchain image"));
        }

        m_CurrentSwapchainImageIndex = static_cast<int8>(renderImageIndex);
        return true;
    }
    bool VulkanSwapchain::presentCurrentImage(VkSemaphore waitSemaphore)
    {
        const uint32 swapchainImageIndex = static_cast<uint8>(m_CurrentSwapchainImageIndex);
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &waitSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &swapchainImageIndex;
        presentInfo.pResults = nullptr;
        const VkResult result = vkQueuePresentKHR(getRenderSubsystemObject()->getQueue(VulkanQueueType::Present)->get(), &presentInfo);
        if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR))
        {
            markAsNeededToRecreate();
            return false;
        }
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to present swapchain image"), result);
            throw std::runtime_error(JSTR("Failed to present swapchain image"));
        }
        return true;
    }
}

#endif