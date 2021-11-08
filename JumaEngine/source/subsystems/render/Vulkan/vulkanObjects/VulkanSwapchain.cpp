// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VulkanSwapchain.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include <glm/common.hpp>
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "subsystems/render/Vulkan/WindowDescription_Vulkan.h"
#include "subsystems/render/Vulkan/Image_Vulkan.h"
#include "utils/jlog.h"
#include "VulkanSwapchainFramebuffer.h"
#include "subsystems/render/Vulkan/RenderOptionsData_Vulkan.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanQueue.h"

namespace JumaEngine
{
    VulkanSwapchain::~VulkanSwapchain()
    {
        if (isValid())
        {
            clearSwapchain();
        }
    }

    bool VulkanSwapchain::init(WindowDescription* window)
    {
        if (isValid() || (window == nullptr))
        {
            return false;
        }

        WindowDescription_Vulkan* window_vulkan = dynamic_cast<WindowDescription_Vulkan*>(window);
        VkPhysicalDevice physicalDevice = getRenderSubsystem()->getPhysicalDevice();

        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, window_vulkan->surface, &capabilities);

        const uint32 imageCount = capabilities.maxImageCount > 0 ? glm::min(capabilities.minImageCount + 1, capabilities.maxImageCount) : capabilities.minImageCount + 1;

        uint32 surfaceFormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, window_vulkan->surface, &surfaceFormatCount, nullptr);
        if (surfaceFormatCount == 0)
        {
            return false;
        }
        jarray<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, window_vulkan->surface, &surfaceFormatCount, surfaceFormats.getData());
        VkSurfaceFormatKHR surfaceFormat = surfaceFormats[0];
        for (const auto& format : surfaceFormats)
        {
            if ((format.format == VK_FORMAT_B8G8R8A8_SRGB) && (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
            {
                surfaceFormat = format;
                break;
            }
        }

        VkFormat depthFormat = VK_FORMAT_UNDEFINED;
        for (const auto& format : jarray<VkFormat>{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT })
        {
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);
            if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            {
                depthFormat = format;
                break;
            }
        }
        if (depthFormat == VK_FORMAT_UNDEFINED)
        {
            JUMA_LOG(error, JSTR("Can't find appropriate depth format"));
            return false;
        }

        const VkExtent2D swapchainExtent = capabilities.currentExtent.width != UINT32_MAX ? capabilities.currentExtent : VkExtent2D{
		    glm::clamp(window->size.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		    glm::clamp(window->size.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
	    };

        uint32 surfacePresentModeCount;
	    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, window_vulkan->surface, &surfacePresentModeCount, nullptr);
	    jarray<VkPresentModeKHR> surfacePresentModes(surfacePresentModeCount);
	    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, window_vulkan->surface, &surfacePresentModeCount, surfacePresentModes.getData());
        const VkPresentModeKHR presentMode = surfacePresentModes.contains(VK_PRESENT_MODE_MAILBOX_KHR) ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR;
        
        m_Window = window;
        m_MaxSampleCount = getMaxSampleCount();
        m_CurrentSettings.sampleCount = m_MaxSampleCount;
        m_CurrentSettings.imageCount = imageCount;
        m_CurrentSettings.surfaceFormat = surfaceFormat;
        m_CurrentSettings.depthFormat = depthFormat;
        m_CurrentSettings.size = { swapchainExtent.width, swapchainExtent.height };
        m_CurrentSettings.presentMode = presentMode;

        if (!createSwapchain() || !createRenderImages() || !createRenderPass() || !createFramebuffers())
        {
            clearSwapchain();
            m_Window = nullptr;
            return false;
        }

        m_SettingForApply = m_CurrentSettings;
        m_Window->onSizeChanged.bind(this, &VulkanSwapchain::onWindowSizeChanged);

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
    WindowDescription_Vulkan* VulkanSwapchain::getWindow() const
    {
        return dynamic_cast<WindowDescription_Vulkan*>(m_Window);
    }

    bool VulkanSwapchain::createSwapchain()
    {
        WindowDescription_Vulkan* window = getWindow();
        
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getRenderSubsystem()->getPhysicalDevice(), window->surface, &capabilities);

        jarray<uint32> queueFamilyIndices;
        queueFamilyIndices.add(getRenderSubsystem()->getQueueFamilyIndex(VulkanQueueType::Graphics));
        queueFamilyIndices.addUnique(getRenderSubsystem()->getQueueFamilyIndex(VulkanQueueType::Present));

        VkSwapchainKHR oldSwapchain = m_Swapchain;
        VkSwapchainCreateInfoKHR swapchainInfo{};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	    swapchainInfo.surface = window->surface;
	    swapchainInfo.minImageCount = m_CurrentSettings.imageCount;
	    swapchainInfo.imageFormat = m_CurrentSettings.surfaceFormat.format;
	    swapchainInfo.imageColorSpace = m_CurrentSettings.surfaceFormat.colorSpace;
	    swapchainInfo.imageExtent = { m_CurrentSettings.size.x, m_CurrentSettings.size.y };
	    swapchainInfo.imageArrayLayers = 1;
	    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	    if (queueFamilyIndices.getSize() > 1)
	    {
		    swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		    swapchainInfo.queueFamilyIndexCount = static_cast<uint32>(queueFamilyIndices.getSize());
		    swapchainInfo.pQueueFamilyIndices = queueFamilyIndices.getData();
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
        const VkResult result = vkCreateSwapchainKHR(getRenderSubsystem()->getDevice(), &swapchainInfo, nullptr, &m_Swapchain);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(error, JSTR("Failed to create swapchain. Code ") + TO_JSTR(result));
            return false;
        }
        if (oldSwapchain != nullptr)
        {
            vkDestroySwapchainKHR(getRenderSubsystem()->getDevice(), oldSwapchain, nullptr);
        }

        vkGetSwapchainImagesKHR(getRenderSubsystem()->getDevice(), m_Swapchain, &m_CurrentSettings.imageCount, nullptr);
        return true;
    }
    bool VulkanSwapchain::createRenderImages()
    {
        m_CurrentSettings.depthFormat = VK_FORMAT_UNDEFINED;
        for (const auto& format : jarray<VkFormat>{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT })
        {
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(getRenderSubsystem()->getPhysicalDevice(), format, &formatProperties);
            if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            {
                m_CurrentSettings.depthFormat = format;
                break;
            }
        }
        if (m_CurrentSettings.depthFormat == VK_FORMAT_UNDEFINED)
        {
            JUMA_LOG(error, JSTR("Can't find appropriate depth format"));
            return false;
        }

        m_RenderImage_Color = jshared_dynamic_cast<Image_Vulkan>(getRenderSubsystem()->createImage());
        m_RenderImage_Color->init(
            m_CurrentSettings.size, 1, m_CurrentSettings.sampleCount, m_CurrentSettings.surfaceFormat.format, 
            { VulkanQueueType::Graphics, VulkanQueueType::Transfer }, 
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VMA_MEMORY_USAGE_GPU_ONLY, 0
        );
        if (!m_RenderImage_Color->isValid() || !m_RenderImage_Color->createImageView(VK_IMAGE_ASPECT_COLOR_BIT))
        {
            return false;
        }

        m_RenderImage_Depth = jshared_dynamic_cast<Image_Vulkan>(getRenderSubsystem()->createImage());
        m_RenderImage_Depth->init(
            m_CurrentSettings.size, 1, m_CurrentSettings.sampleCount, m_CurrentSettings.depthFormat,
            { VulkanQueueType::Graphics, VulkanQueueType::Transfer },
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VMA_MEMORY_USAGE_GPU_ONLY, 0
        );
        if (!m_RenderImage_Depth->isValid() || !m_RenderImage_Depth->createImageView(VK_IMAGE_ASPECT_DEPTH_BIT))
        {
            return false;
        }

        return true;
    }
    bool VulkanSwapchain::createRenderPass()
    {
        const bool multisampleEnabled = m_CurrentSettings.sampleCount != VK_SAMPLE_COUNT_1_BIT;

        jarray<VkAttachmentDescription> attachments;
        VkAttachmentDescription& colorAttachment = attachments.addDefault();
        colorAttachment.format = m_CurrentSettings.surfaceFormat.format;
        colorAttachment.samples = m_CurrentSettings.sampleCount;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = multisampleEnabled ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        VkAttachmentDescription& depthAttachment = attachments.addDefault();
        depthAttachment.format = Image_Vulkan::getVulkanFormatByImageFormat(m_RenderImage_Depth->getFormat());
        depthAttachment.samples = m_CurrentSettings.sampleCount;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        if (multisampleEnabled)
        {
            VkAttachmentDescription& colorAttachmentResolve = attachments.addDefault();
            colorAttachmentResolve.format = m_CurrentSettings.surfaceFormat.format;
            colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }

        jarray<VkAttachmentReference> attachmentRefs;
        VkAttachmentReference& colorAttachmentRef = attachmentRefs.addDefault();
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkAttachmentReference& depthAttachmentRef = attachmentRefs.addDefault();
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        if (multisampleEnabled)
        {
            VkAttachmentReference& colorAttachmentResolveRef = attachmentRefs.addDefault();
            colorAttachmentResolveRef.attachment = 2;
            colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &attachmentRefs[0];
        subpass.pDepthStencilAttachment = &attachmentRefs[1];
        if (multisampleEnabled)
        {
            subpass.pResolveAttachments = &attachmentRefs[2];
        }

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo = VkRenderPassCreateInfo();
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32>(attachments.getSize());
        renderPassInfo.pAttachments = attachments.getData();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        const VkResult result = vkCreateRenderPass(getRenderSubsystem()->getDevice(), &renderPassInfo, nullptr, &m_RenderPass);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(error, JSTR("Failed to create render pass. Code ") + TO_JSTR(result));
            return false;
        }
        return true;
    }
    bool VulkanSwapchain::createFramebuffers()
    {
        uint32 swapchainImageCount = 0;
        vkGetSwapchainImagesKHR(getRenderSubsystem()->getDevice(), m_Swapchain, &swapchainImageCount, nullptr);
        jarray<VkImage> swapchainImages(swapchainImageCount);
	    vkGetSwapchainImagesKHR(getRenderSubsystem()->getDevice(), m_Swapchain, &swapchainImageCount, swapchainImages.getData());

        m_Framebuffers.resize(swapchainImageCount);
        for (uint32 index = 0; index < m_Framebuffers.getSize(); index++)
        {
            if (m_Framebuffers[index] == nullptr)
            {
                m_Framebuffers[index] = getRenderSubsystem()->createVulkanObject<VulkanSwapchainFramebuffer>();
            }
            m_Framebuffers[index]->setCommandBuffer(nullptr);
            m_Framebuffers[index]->setRenderFinishedFence(nullptr);
            if (!m_Framebuffers[index]->init(this, swapchainImages[index]))
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
                JUMA_LOG(error, JSTR("Failed to create semaphore. Code ") + TO_JSTR(result));
                return false;
            }
            result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_Semaphores_RenderFinished[index]);
            if (result != VK_SUCCESS)
            {
                JUMA_LOG(error, JSTR("Failed to create semaphore. Code ") + TO_JSTR(result));
                return false;
            }
            result = vkCreateFence(device, &fenceInfo, nullptr, &m_Fences_RenderFinished[index]);
            if (result != VK_SUCCESS)
            {
                JUMA_LOG(error, JSTR("Failed to create fence. Code ") + TO_JSTR(result));
                return false;
            }
        }

        m_CurrentInFlightFrame = glm::clamp<uint32>(m_CurrentInFlightFrame, 0, m_MaxFramesInFlight - 1);
        return true;
    }

    void VulkanSwapchain::clearInternal()
    {
        clearSwapchain();

        if (m_Window != nullptr)
        {
            m_Window->onSizeChanged.unbind(this, &VulkanSwapchain::onWindowSizeChanged);
            m_Window = nullptr;
        }
    }
    void VulkanSwapchain::clearSwapchain()
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

        m_Framebuffers.clear();

        if (m_RenderPass != nullptr)
        {
            vkDestroyRenderPass(device, m_RenderPass, nullptr);
            m_RenderPass = nullptr;

            onRenderPassChanged.call(this);
        }

        m_RenderImage_Depth.reset();
        m_RenderImage_Color.reset();

        if (m_Swapchain != nullptr)
        {
            vkDestroySwapchainKHR(device, m_Swapchain, nullptr);
            m_Swapchain = nullptr;
        }
    }

    void VulkanSwapchain::onWindowSizeChanged(WindowDescription* window)
    {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getRenderSubsystem()->getPhysicalDevice(), getWindow()->surface, &capabilities);
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            m_SettingForApply.size = { capabilities.currentExtent.width, capabilities.currentExtent.height };
        }
        else
        {
            m_SettingForApply.size = {
		        glm::clamp(window->size.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		        glm::clamp(window->size.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
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
            clearSwapchain();
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

        if (shouldRecreateRenderPass)
        {
            vkDestroyRenderPass(getRenderSubsystem()->getDevice(), m_RenderPass, nullptr);
        }
        m_CurrentSettings = m_SettingForApply;
        if (shouldRecreateSwapchain && !createSwapchain())
        {
            return false;
        }
        if (shouldRecreateRenderImages && !createRenderImages())
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

        uint32 renderImageIndex = 0;
        vkWaitForFences(device, 1, &m_Fences_RenderFinished[m_CurrentInFlightFrame], VK_TRUE, UINT64_MAX);
        const VkResult result = vkAcquireNextImageKHR(device, m_Swapchain, UINT64_MAX, m_Semaphores_ImageAvailable[m_CurrentInFlightFrame], nullptr, &renderImageIndex);
        if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR))
        {
            if (result != VK_ERROR_OUT_OF_DATE_KHR)
            {
                const jstring message = JSTR("Failed to acquire swapchain image. Code ") + TO_JSTR(result);
                JUMA_LOG(error, message);
                throw std::runtime_error(message);
            }

            markAsNeededToRecreate();
            return false;
        }
        VkFence fence = m_Framebuffers[renderImageIndex]->getRenderFinishedFence();
        if (fence != nullptr)
        {
            vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
        }

        const jshared_ptr<VulkanCommandBuffer> commandBuffer = createRenderCommandBuffer(renderImageIndex);
        if (commandBuffer == nullptr)
        {
            return false;
        }
        m_Framebuffers[renderImageIndex]->setCommandBuffer(commandBuffer);
        m_Framebuffers[renderImageIndex]->setRenderFinishedFence(m_Fences_RenderFinished[m_CurrentInFlightFrame]);
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
            throw std::runtime_error(message);
        }

        jarray<VkClearValue> clearValues(2);
        clearValues[0].color = { { 1.0f, 1.0f, 1.0f, 1.0f } };
        clearValues[1].depthStencil = { 1.0f, 0 };
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_RenderPass;
        renderPassInfo.framebuffer = m_Framebuffers[swapchainImageIndex]->get();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = { m_CurrentSettings.size.x, m_CurrentSettings.size.y };
        renderPassInfo.clearValueCount = static_cast<uint32>(clearValues.getSize());
        renderPassInfo.pClearValues = clearValues.getData();
        vkCmdBeginRenderPass(commandBuffer->get(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        return commandBuffer;
    }

    void VulkanSwapchain::finishRender(const RenderOptions& options)
    {
        const uint32 swapchainImageIndex = options.getData<RenderOptionsData_Vulkan>()->swapchainImageIndex;
        const jshared_ptr<VulkanCommandBuffer>& commandBuffer = m_Framebuffers[swapchainImageIndex]->getCommandBuffer();

        vkCmdEndRenderPass(commandBuffer->get());
        VkResult result = vkEndCommandBuffer(commandBuffer->get());
        if (result != VK_SUCCESS) 
        {
            const jstring message = JSTR("Failed to record command buffer. Code ") + TO_JSTR(result);
            JUMA_LOG(error, message);
            throw std::runtime_error(message);
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
            throw std::runtime_error(message);
        }

        m_CurrentInFlightFrame = (m_CurrentInFlightFrame + 1) % m_MaxFramesInFlight;
    }
}

#endif