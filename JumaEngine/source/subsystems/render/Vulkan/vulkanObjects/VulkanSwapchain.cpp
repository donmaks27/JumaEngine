// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VulkanSwapchain.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "VulkanQueue.h"
#include "VulkanRenderImage.h"
#include "subsystems/render/Vulkan/RenderOptions_Vulkan.h"
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

        // Pick depth format
        VkFormat depthFormat = VK_FORMAT_UNDEFINED;
        if (!getRenderSubsystem()->pickDepthFormat(depthFormat))
        {
            JUMA_LOG(error, JSTR("Can't find appropriate depth format"));
            return false;
        }

        // Calculate swapchain size
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getRenderSubsystem()->getPhysicalDevice(), surface, &capabilities);
        const VkExtent2D swapchainSize = capabilities.currentExtent.width != UINT32_MAX ? capabilities.currentExtent : VkExtent2D{
		    math::clamp(windowSize.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		    math::clamp(windowSize.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
	    };

        m_WindowSurface = surface;
        m_MaxSampleCount = getMaxSampleCount();
        m_CurrentSettings.sampleCount = m_MaxSampleCount;
        m_CurrentSettings.surfaceFormat = surfaceFormat;
        m_CurrentSettings.depthFormat = depthFormat;
        m_CurrentSettings.size = { swapchainSize.width, swapchainSize.height };
        m_CurrentSettings.presentMode = getRenderSubsystem()->getPresentMode();

        if (!updateSwapchain() || !updateRenderPass() || !updateSyncObjects())
        {
            clearVulkanObjects();
            m_WindowSurface = nullptr;
            return false;
        }

        m_SettingForApply = m_CurrentSettings;
        markAsInitialized();
        return true;
    }

    VkSampleCountFlagBits VulkanSwapchain::getMaxSampleCount() const
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(getRenderSubsystem()->getPhysicalDevice(), &deviceProperties);

        const VkSampleCountFlags counts = deviceProperties.limits.framebufferColorSampleCounts & deviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
        return VK_SAMPLE_COUNT_1_BIT;
    }

    bool VulkanSwapchain::updateSwapchain()
    {
        VkDevice device = getRenderSubsystem()->getDevice();
        
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getRenderSubsystem()->getPhysicalDevice(), m_WindowSurface, &capabilities);
        uint32 imageCount = getRenderSubsystem()->getPresentMode() != RenderPresentMode::TRIPLE_BUFFER ? 3 : 2;
        if (capabilities.maxImageCount > 0)
        {
            imageCount = math::min(imageCount, capabilities.maxImageCount);
        }
        imageCount = math::max(capabilities.minImageCount, imageCount);

        const uint32 queueFamilyIndices[2] = {
            getRenderSubsystem()->getQueueFamilyIndex(VulkanQueueType::Graphics),
            getRenderSubsystem()->getQueueFamilyIndex(VulkanQueueType::Present)
        };

        VkSwapchainKHR oldSwapchain = m_Swapchain;
        VkSwapchainCreateInfoKHR swapchainInfo{};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	    swapchainInfo.surface = m_WindowSurface;
	    swapchainInfo.minImageCount = imageCount;
	    swapchainInfo.imageFormat = m_CurrentSettings.surfaceFormat.format;
	    swapchainInfo.imageColorSpace = m_CurrentSettings.surfaceFormat.colorSpace;
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
	    swapchainInfo.presentMode = getVulkanPresentMode(m_CurrentSettings.presentMode);
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
    bool VulkanSwapchain::updateRenderPass()
    {
        VulkanRenderPassDescription description;
        description.sampleCount = m_CurrentSettings.sampleCount;
        description.colorFormat = m_CurrentSettings.surfaceFormat.format;
        description.depthFormat = m_CurrentSettings.depthFormat;
        description.renderToSwapchain = true;
        m_RenderPass = getRenderSubsystem()->createRenderPass(description);
        if (m_RenderPass == nullptr)
        {
            JUMA_LOG(error, JSTR("Failed to create render pass"));
            return false;
        }
        return true;
    }
    bool VulkanSwapchain::updateSyncObjects()
    {
        constexpr int8 maxFrameCount = RenderSubsystem_Vulkan::getMaxRenderFrameCount();
        if (m_RenderFrameAvailableSemaphores.getSize() == maxFrameCount)
        {
            return true;
        }

        VkDevice device = getRenderSubsystem()->getDevice();
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (int32 index = m_RenderFrameAvailableSemaphores.getSize() - 1; index >= maxFrameCount; index--)
        {
            if (m_RenderFrameAvailableSemaphores[index] != nullptr)
            {
                vkDestroySemaphore(device, m_RenderFrameAvailableSemaphores[index], nullptr);
            }
        }
        m_RenderFrameAvailableSemaphores.resize(maxFrameCount);
        for (auto& semaphore : m_RenderFrameAvailableSemaphores)
        {
            if (semaphore == nullptr)
            {
                const VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore);
                if (result != VK_SUCCESS)
                {
                    JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create semaphore"), result);
                    return false;
                }
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
        VkDevice device = getRenderSubsystem()->getDevice();

        for (const auto& semaphore : m_RenderFrameAvailableSemaphores)
        {
            if (semaphore != nullptr)
            {
                vkDestroySemaphore(device, semaphore, nullptr);
            }
        }
        m_RenderFrameAvailableSemaphores.clear();

        if (m_RenderPass != nullptr)
        {
            m_RenderPass = nullptr;
            onRenderPassChanged.call(this);
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
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getRenderSubsystem()->getPhysicalDevice(), m_WindowSurface, &capabilities);
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
        vkDeviceWaitIdle(getRenderSubsystem()->getDevice());
        m_NeedToRecreate = false;

        const bool sizeChanged = forceRecreate || (m_SettingForApply.size != m_CurrentSettings.size);
        const bool colorFormatChanged = forceRecreate || (m_SettingForApply.surfaceFormat.format != m_CurrentSettings.surfaceFormat.format) || (m_SettingForApply.surfaceFormat.colorSpace != m_CurrentSettings.surfaceFormat.colorSpace);
        const bool depthFormatChanged = forceRecreate || (m_SettingForApply.depthFormat != m_CurrentSettings.depthFormat);
        const bool presentModeChanged = forceRecreate || (m_SettingForApply.presentMode != m_CurrentSettings.presentMode);
        const bool sampleCountChanged = forceRecreate || (m_SettingForApply.sampleCount != m_CurrentSettings.sampleCount);

        const bool shouldRecreateSwapchain = sizeChanged || colorFormatChanged || presentModeChanged;
        const bool shouldRecreateRenderPass = colorFormatChanged || depthFormatChanged || sampleCountChanged;
        const bool shouldRecreateRenderImage = shouldRecreateSwapchain || shouldRecreateRenderPass;

        m_CurrentSettings = m_SettingForApply;
        if (shouldRecreateSwapchain && !updateSwapchain())
        {
            return false;
        }
        if (shouldRecreateRenderPass && !updateRenderPass())
        {
            return false;
        }
        if (shouldRecreateRenderImage && !updateSyncObjects())
        {
            return false;
        }

        if (shouldRecreateRenderPass)
        {
            onRenderPassChanged.call(this);
        }
        return true;
    }

    bool VulkanSwapchain::startRender(VulkanRenderImage* renderImage, RenderOptions* options)
    {
        if (!isValid() || m_NeedToRecreate || (renderImage == nullptr))
        {
            return false;
        }

        VkDevice device = getRenderSubsystem()->getDevice();

        VkFence prevFrameFence = renderImage->getFinishFence();
        if (prevFrameFence != nullptr)
        {
            vkWaitForFences(device, 1, &prevFrameFence, VK_TRUE, UINT64_MAX);
        }

        const int8 renderFrameIndex = getRenderSubsystem()->getNextRenderFrameIndex();
        uint32 renderImageIndex = 0;
        const VkResult result = vkAcquireNextImageKHR(device, m_Swapchain, UINT64_MAX, m_RenderFrameAvailableSemaphores[renderFrameIndex], nullptr, &renderImageIndex);
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

        VulkanCommandBuffer* commandBuffer = renderImage->startRenderCommandBufferRecord();
        if (commandBuffer == nullptr)
        {
            return false;
        }

        RenderOptions_Vulkan* renderOptions = reinterpret_cast<RenderOptions_Vulkan*>(options);
        renderOptions->renderImage = renderImage;
        return true;
    }

    void VulkanSwapchain::finishRender(VulkanRenderImage* renderImage, RenderOptions* options)
    {
        RenderOptions_Vulkan* renderOptions = reinterpret_cast<RenderOptions_Vulkan*>(options);

        renderImage->submitRenderCommandBuffer();

        const uint32 swapchainImageIndex = static_cast<uint8>(m_CurrentSwapchainImageIndex);
        VkSemaphore finishSemaphore = renderImage->getFinishSemaphore();
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &finishSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &swapchainImageIndex;
        presentInfo.pResults = nullptr;
        const VkResult result = vkQueuePresentKHR(getRenderSubsystem()->getQueue(VulkanQueueType::Present)->get(), &presentInfo);
        if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR))
        {
            markAsNeededToRecreate();
        }
        else if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to present swapchain image"), result);
            throw std::runtime_error(JSTR("Failed to present swapchain image"));
        }
    }
}

#endif