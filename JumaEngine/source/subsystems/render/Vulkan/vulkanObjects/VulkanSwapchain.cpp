// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VulkanSwapchain.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "subsystems/render/Vulkan/Image_Vulkan.h"
#include "jutils/jlog.h"
#include "subsystems/render/Vulkan/RenderOptionsData_Vulkan.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanQueue.h"
#include "subsystems/window/Vulkan/Window_Vulkan.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"

namespace JumaEngine
{
    VulkanSwapchain::~VulkanSwapchain()
    {
        if (isValid())
        {
            clearVulkanObjects();
        }
    }

    bool VulkanSwapchain::init(Window_Vulkan* window)
    {
        if (isValid())
        {
            JUMA_LOG(error, JSTR("Swapchain already initialized"));
            return false;
        }
        if (window == nullptr)
        {
            JUMA_LOG(error, JSTR("Invalid window parameter"));
            return false;
        }

        RenderSubsystem_Vulkan* renderSubsystem = getRenderSubsystem();
        VkPhysicalDevice physicalDevice = renderSubsystem->getPhysicalDevice();
        VkSurfaceKHR surface = window->getVulkanSurface();

        // Get image count
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
        const uint32 imageCount = capabilities.maxImageCount > 0 ? math::min(capabilities.minImageCount + 1, capabilities.maxImageCount) : capabilities.minImageCount + 1;

        // Pick surface format
        VkSurfaceFormatKHR surfaceFormat;
        if (!window->pickSurfaceFormat(surfaceFormat))
        {
            JUMA_LOG(error, JSTR("Failed to pick surface format"));
            return false;
        }

        // Pick depth format
        VkFormat depthFormat = VK_FORMAT_UNDEFINED;
        if (!renderSubsystem->pickDepthFormat(depthFormat))
        {
            JUMA_LOG(error, JSTR("Can't find appropriate depth format"));
            return false;
        }

        // Calculate swapchain size
        const math::uvector2 windowSize = window->getSize();
        const VkExtent2D swapchainSize = capabilities.currentExtent.width != UINT32_MAX ? capabilities.currentExtent : VkExtent2D{
		    math::clamp(windowSize.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		    math::clamp(windowSize.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
	    };

        // Pick present mode
        uint32 surfacePresentModeCount;
	    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, nullptr);
	    jarray<VkPresentModeKHR> surfacePresentModes(surfacePresentModeCount);
	    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, surfacePresentModes.getData());
        const VkPresentModeKHR presentMode = surfacePresentModes.contains(VK_PRESENT_MODE_MAILBOX_KHR) ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR;
        
        m_Window = window;
        m_MaxSampleCount = getMaxSampleCount();
        m_CurrentSettings.sampleCount = m_MaxSampleCount;
        m_CurrentSettings.imageCount = imageCount;
        m_CurrentSettings.surfaceFormat = surfaceFormat;
        m_CurrentSettings.depthFormat = depthFormat;
        m_CurrentSettings.size = { swapchainSize.width, swapchainSize.height };
        m_CurrentSettings.presentMode = presentMode;

        if (!createSwapchain() || !createRenderPass() || !createFramebuffers())
        {
            clearVulkanObjects();
            m_Window = nullptr;
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

    bool VulkanSwapchain::createSwapchain()
    {
        VkDevice device = getRenderSubsystem()->getDevice();
        VkSurfaceKHR surface = m_Window->getVulkanSurface();
        
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getRenderSubsystem()->getPhysicalDevice(), surface, &capabilities);

        const uint32 queueFamilyIndices[2] = {
            getRenderSubsystem()->getQueueFamilyIndex(VulkanQueueType::Graphics),
            getRenderSubsystem()->getQueueFamilyIndex(VulkanQueueType::Present)
        };

        VkSwapchainKHR oldSwapchain = m_Swapchain;
        VkSwapchainCreateInfoKHR swapchainInfo{};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	    swapchainInfo.surface = surface;
	    swapchainInfo.minImageCount = m_CurrentSettings.imageCount;
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
	    swapchainInfo.presentMode = m_CurrentSettings.presentMode;
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

        vkGetSwapchainImagesKHR(device, m_Swapchain, &m_CurrentSettings.imageCount, nullptr);
        return true;
    }
    bool VulkanSwapchain::createRenderPass()
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
    bool VulkanSwapchain::createFramebuffers()
    {
        RenderSubsystem_Vulkan* renderSubsystem = getRenderSubsystem();

        uint32 swapchainImageCount = m_CurrentSettings.imageCount;
        jarray<VkImage> swapchainImages(swapchainImageCount);
	    vkGetSwapchainImagesKHR(getRenderSubsystem()->getDevice(), m_Swapchain, &swapchainImageCount, swapchainImages.getData());

        for (int32 index = swapchainImageCount; index < m_Framebuffers.getSize(); index++)
        {
            delete m_Framebuffers[index];
        }
        m_Framebuffers.resize(swapchainImageCount);
        m_InFlightFrameIndices.resize(swapchainImageCount);

        for (int32 index = 0; index < m_Framebuffers.getSize(); index++)
        {
            m_InFlightFrameIndices[index] = -1;

            if (m_Framebuffers[index] == nullptr)
            {
                m_Framebuffers[index] = renderSubsystem->createVulkanObject<VulkanFramebuffer>();
            }
            m_Framebuffers[index]->setRenderCommandBuffer(nullptr);
            if (!m_Framebuffers[index]->create(m_RenderPass, m_CurrentSettings.size, swapchainImages[index]))
            {
                return false;
            }
        }
        return createSyncObjects();
    }
    bool VulkanSwapchain::createSyncObjects()
    {
        VkDevice device = getRenderSubsystem()->getDevice();

        m_Semaphores_ImageAvailable.resize(m_MaxFramesInFlight);
        m_Semaphores_RenderFinished.resize(m_MaxFramesInFlight);
        m_Fences_RenderFinished.resize(m_MaxFramesInFlight);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (uint32 index = 0; index < m_MaxFramesInFlight; index++)
        {
            if (m_Semaphores_ImageAvailable[index] != nullptr)
            {
                vkDestroySemaphore(device, m_Semaphores_ImageAvailable[index], nullptr);
            }
            if (m_Semaphores_RenderFinished[index] != nullptr)
            {
                vkDestroySemaphore(device, m_Semaphores_RenderFinished[index], nullptr);
            }
            if (m_Fences_RenderFinished[index] != nullptr)
            {
                vkDestroyFence(device, m_Fences_RenderFinished[index], nullptr);
            }

            VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_Semaphores_ImageAvailable[index]);
            if (result != VK_SUCCESS)
            {
                JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create semaphore"), result);
                return false;
            }
            result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_Semaphores_RenderFinished[index]);
            if (result != VK_SUCCESS)
            {
                JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create semaphore"), result);
                return false;
            }
            result = vkCreateFence(device, &fenceInfo, nullptr, &m_Fences_RenderFinished[index]);
            if (result != VK_SUCCESS)
            {
                JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create fence"), result);
                return false;
            }
        }

        m_CurrentInFlightFrame = math::clamp(m_CurrentInFlightFrame, 0, m_MaxFramesInFlight - 1);
        return true;
    }

    void VulkanSwapchain::clearInternal()
    {
        clearVulkanObjects();

        m_Window = nullptr;
    }
    void VulkanSwapchain::clearVulkanObjects()
    {
        VkDevice device = getRenderSubsystem()->getDevice();

        for (const auto& semaphore : m_Semaphores_ImageAvailable)
        {
            if (semaphore != nullptr)
            {
                vkDestroySemaphore(device, semaphore, nullptr);
            }
        }
        for (const auto& semaphore : m_Semaphores_RenderFinished)
        {
            if (semaphore != nullptr)
            {
                vkDestroySemaphore(device, semaphore, nullptr);
            }
        }
        for (const auto& fence : m_Fences_RenderFinished)
        {
            if (fence != nullptr)
            {
                vkDestroyFence(device, fence, nullptr);
            }
        }
        m_Semaphores_ImageAvailable.clear();
        m_Semaphores_RenderFinished.clear();
        m_Fences_RenderFinished.clear();

        for (auto& frambuffer : m_Framebuffers)
        {
            delete frambuffer;
        }
        m_Framebuffers.clear();

        if (m_RenderPass != nullptr)
        {
            m_RenderPass = nullptr;
            onRenderPassChanged.call(this);
        }

        if (m_Swapchain != nullptr)
        {
            vkDestroySwapchainKHR(device, m_Swapchain, nullptr);
            m_Swapchain = nullptr;
        }
    }

    void VulkanSwapchain::onWindowSizeChanged()
    {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getRenderSubsystem()->getPhysicalDevice(), m_Window->getVulkanSurface(), &capabilities);
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            m_SettingForApply.size = { capabilities.currentExtent.width, capabilities.currentExtent.height };
        }
        else
        {
            const math::uvector2 windowSize = m_Window->getSize();
            m_SettingForApply.size = {
		        math::clamp(windowSize.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		        math::clamp(windowSize.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
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
        const bool imageCountChanged = forceRecreate || (m_SettingForApply.imageCount != m_CurrentSettings.imageCount);
        const bool colorFormatChanged = forceRecreate || (m_SettingForApply.surfaceFormat.format != m_CurrentSettings.surfaceFormat.format) || (m_SettingForApply.surfaceFormat.colorSpace != m_CurrentSettings.surfaceFormat.colorSpace);
        const bool depthFormatChanged = forceRecreate || (m_SettingForApply.depthFormat != m_CurrentSettings.depthFormat);
        const bool presentModeChanged = forceRecreate || (m_SettingForApply.presentMode != m_CurrentSettings.presentMode);
        const bool sampleCountChanged = forceRecreate || (m_SettingForApply.sampleCount != m_CurrentSettings.sampleCount);

        const bool shouldRecreateSwapchain = sizeChanged || imageCountChanged || colorFormatChanged || presentModeChanged;
        const bool shouldRecreateRenderImages = sizeChanged || colorFormatChanged || depthFormatChanged || sampleCountChanged;
        const bool shouldRecreateRenderPass = colorFormatChanged || depthFormatChanged || sampleCountChanged;
        const bool shouldRecreateFramebuffers = shouldRecreateSwapchain || shouldRecreateRenderImages || shouldRecreateRenderPass;

        m_CurrentSettings = m_SettingForApply;
        if (shouldRecreateSwapchain && !createSwapchain())
        {
            return false;
        }
        if (shouldRecreateRenderPass && !createRenderPass())
        {
            return false;
        }
        if (shouldRecreateFramebuffers && !createFramebuffers())
        {
            return false;
        }

        if (shouldRecreateRenderPass)
        {
            onRenderPassChanged.call(this);
        }
        return true;
    }

    bool VulkanSwapchain::startRender(const RenderOptions& options)
    {
        if (!isValid() || m_NeedToRecreate)
        {
            return false;
        }

        VkDevice device = getRenderSubsystem()->getDevice();

        vkWaitForFences(device, 1, &m_Fences_RenderFinished[m_CurrentInFlightFrame], VK_TRUE, UINT64_MAX);

        uint32 renderImageIndex = 0;
        const VkResult result = vkAcquireNextImageKHR(device, m_Swapchain, UINT64_MAX, m_Semaphores_ImageAvailable[m_CurrentInFlightFrame], nullptr, &renderImageIndex);
        if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR))
        {
            if (result != VK_ERROR_OUT_OF_DATE_KHR)
            {
                const jstring message = JSTR("Failed to acquire swapchain image. Code ") + TO_JSTR(result);
                JUMA_LOG(error, message);
                throw std::runtime_error(*message);
            }

            markAsNeededToRecreate();
            return false;
        }

        const int32 prevInFlightFrameIndex = m_InFlightFrameIndices[renderImageIndex];
        VkFence fence = m_Fences_RenderFinished.isValidIndex(prevInFlightFrameIndex) ? m_Fences_RenderFinished[prevInFlightFrameIndex] : nullptr;
        if (fence != nullptr)
        {
            vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
            m_InFlightFrameIndices[renderImageIndex] = -1;
        }

        const jshared_ptr<VulkanCommandBuffer> commandBuffer = createRenderCommandBuffer(renderImageIndex);
        if (commandBuffer == nullptr)
        {
            return false;
        }
        m_Framebuffers[renderImageIndex]->setRenderCommandBuffer(commandBuffer);
        m_InFlightFrameIndices[renderImageIndex] = m_CurrentInFlightFrame;
        vkResetFences(device, 1, &m_Fences_RenderFinished[m_CurrentInFlightFrame]);

        RenderOptionsData_Vulkan* optionsData = options.getData<RenderOptionsData_Vulkan>();
        optionsData->swapchainImageIndex = renderImageIndex;
        optionsData->commandBuffer = commandBuffer;
        return true;
    }
    jshared_ptr<VulkanCommandBuffer> VulkanSwapchain::createRenderCommandBuffer(const uint32 swapchainImageIndex)
    {
        const jshared_ptr<VulkanCommandPool> commandPool = getRenderSubsystem()->getCommandPool(VulkanQueueType::Graphics);
        jshared_ptr<VulkanCommandBuffer> commandBuffer = commandPool != nullptr ? commandPool->createCommandBuffer(true) : nullptr;
        if ((commandBuffer == nullptr) || !commandBuffer->isValid())
        {
            return nullptr;
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;
        const VkResult result = vkBeginCommandBuffer(commandBuffer->get(), &beginInfo);
        if (result != VK_SUCCESS)
        {
            const jstring message = JSTR("Failed to begin recording command buffer. Code ") + TO_JSTR(result);
            JUMA_LOG(error, message);
            throw std::runtime_error(*message);
        }

        VkClearValue clearValues[2];
        clearValues[0].color = { { 1.0f, 1.0f, 1.0f, 1.0f } };
        clearValues[1].depthStencil = { 1.0f, 0 };
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_RenderPass->get();
        renderPassInfo.framebuffer = m_Framebuffers[swapchainImageIndex]->get();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = { m_CurrentSettings.size.x, m_CurrentSettings.size.y };
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues;
        vkCmdBeginRenderPass(commandBuffer->get(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_CurrentSettings.size.x);
        viewport.height = static_cast<float>(m_CurrentSettings.size.y);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = { m_CurrentSettings.size.x, m_CurrentSettings.size.y };
        vkCmdSetViewport(commandBuffer->get(), 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer->get(), 0, 1, &scissor);

        return commandBuffer;
    }

    void VulkanSwapchain::finishRender(const RenderOptions& options)
    {
        const uint32 swapchainImageIndex = options.getData<RenderOptionsData_Vulkan>()->swapchainImageIndex;
        const jshared_ptr<VulkanCommandBuffer>& commandBuffer = m_Framebuffers[swapchainImageIndex]->getRenderCommandBuffer();

        vkCmdEndRenderPass(commandBuffer->get());
        VkResult result = vkEndCommandBuffer(commandBuffer->get());
        if (result != VK_SUCCESS) 
        {
            const jstring message = JSTR("Failed to record command buffer. Code ") + TO_JSTR(result);
            JUMA_LOG(error, message);
            throw std::runtime_error(*message);
        }

        VkSemaphore waitSemaphores[] = { m_Semaphores_ImageAvailable[m_CurrentInFlightFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSemaphore signalSemaphores[] = { m_Semaphores_RenderFinished[m_CurrentInFlightFrame] };
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        if (!commandBuffer->submit(submitInfo, m_Fences_RenderFinished[m_CurrentInFlightFrame], false))
        {
            throw std::runtime_error(JSTR("Failed to submit draw command buffer"));
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &swapchainImageIndex;
        presentInfo.pResults = nullptr;
        result = vkQueuePresentKHR(getRenderSubsystem()->getQueue(VulkanQueueType::Present)->get(), &presentInfo);
        if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR))
        {
            markAsNeededToRecreate();
        }
        else if (result != VK_SUCCESS)
        {
            const jstring message = JSTR("Failed to present swapchain image. Code ") + TO_JSTR(result);
            JUMA_LOG(error, message);
            throw std::runtime_error(*message);
        }

        m_CurrentInFlightFrame = (m_CurrentInFlightFrame + 1) % m_MaxFramesInFlight;
    }
}

#endif