// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VulkanSwapchain.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include <glm/common.hpp>
#include "RenderSubsystem_Vulkan.h"
#include "WindowDescription_Vulkan.h"
#include "Image_Vulkan.h"
#include "utils/jlog.h"
#include "VulkanSwapchainFramebuffer.h"

namespace JumaEngine
{
    VulkanSwapchain::~VulkanSwapchain()
    {
        if (isValid())
        {
            clearSwapchain();
        }
    }

    bool VulkanSwapchain::init(const jshared_ptr<WindowDescription>& window)
    {
        if (isValid() || (window == nullptr))
        {
            return false;
        }

        const jshared_ptr<WindowDescription_Vulkan> window_vulkan = jshared_dynamic_cast<WindowDescription_Vulkan>(window);
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
    jshared_ptr<WindowDescription_Vulkan> VulkanSwapchain::getWindow() const
    {
        return jshared_dynamic_cast<WindowDescription_Vulkan>(m_Window);
    }

    bool VulkanSwapchain::createSwapchain()
    {
        const jshared_ptr<WindowDescription_Vulkan> window = getWindow();
        
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
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_CurrentSettings.surfaceFormat.format;
        colorAttachment.samples = m_CurrentSettings.sampleCount;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = Image_Vulkan::getVulkanFormatByImageFormat(m_RenderImage_Depth->getFormat());
        depthAttachment.samples = m_CurrentSettings.sampleCount;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        VkAttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = m_CurrentSettings.surfaceFormat.format;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        jarray<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        VkAttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;

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
        for (uint32 index = 0; index < m_Framebuffers.getSize(); ++index)
        {
            if (m_Framebuffers[index] == nullptr)
            {
                m_Framebuffers[index] = getRenderSubsystem()->createVulkanObject<VulkanSwapchainFramebuffer>();
            }
            if (!m_Framebuffers[index]->init(this, swapchainImages[index]))
            {
                return false;
            }
        }
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

        m_Framebuffers.clear();

        if (m_RenderPass != nullptr)
        {
            vkDestroyRenderPass(device, m_RenderPass, nullptr);
            m_RenderPass = nullptr;
        }

        m_RenderImage_Depth.reset();
        m_RenderImage_Color.reset();

        if (m_Swapchain != nullptr)
        {
            vkDestroySwapchainKHR(device, m_Swapchain, nullptr);
            m_Swapchain = nullptr;
        }
    }

    void VulkanSwapchain::onWindowSizeChanged(const jshared_ptr<WindowDescription>& window)
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
    }

    void VulkanSwapchain::applySettings(const bool forceRecreate)
    {
        if (!isValid())
        {
            return;
        }
        if (!applySettingsInternal(forceRecreate))
        {
            clear();
        }
    }
    bool VulkanSwapchain::applySettingsInternal(const bool forceRecreate)
    {
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

        return true;
    }
}

#endif