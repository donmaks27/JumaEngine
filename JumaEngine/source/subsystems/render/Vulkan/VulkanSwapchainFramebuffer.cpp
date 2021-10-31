// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VulkanSwapchainFramebuffer.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "utils/jlog.h"
#include "RenderSubsystem_Vulkan.h"
#include "Image_Vulkan.h"
#include "VulkanSwapchain.h"

namespace JumaEngine
{
    VulkanSwapchainFramebuffer::~VulkanSwapchainFramebuffer()
    {
        if (isValid())
        {
            clearFramebuffer();
        }
    }

    bool VulkanSwapchainFramebuffer::init(VulkanSwapchain* swapchain, VkImage swapchainImage)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Framebuffer already initialized"));
            return false;
        }
        if ((swapchain == nullptr) || (swapchainImage == nullptr))
        {
            JUMA_LOG(warning, JSTR("Wrong input data"));
            return false;
        }

        m_Image = jshared_dynamic_cast<Image_Vulkan>(getRenderSubsystem()->createImage());
        m_Image->init(swapchainImage, swapchain->getSize(), Image_Vulkan::getImageFormatByVulkanFormat(swapchain->getFormat()), 1);
        m_Image->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);

        const glm::uvec2 swapchainSize = swapchain->getSize();
        const jarray<VkImageView> attachments = { swapchain->getRenderColorImage()->getImageView(), swapchain->getRenderDepthImage()->getImageView(), m_Image->getImageView() };
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = swapchain->getRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32>(attachments.getSize());
        framebufferInfo.pAttachments = attachments.getData();
        framebufferInfo.width = swapchainSize.x;
        framebufferInfo.height = swapchainSize.y;
        framebufferInfo.layers = 1;
        const VkResult result = vkCreateFramebuffer(getRenderSubsystem()->getDevice(), &framebufferInfo, nullptr, &m_Framebuffer);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(error, JSTR("Failed to create framebuffer. Code ") + TO_JSTR(result));
            return false;
        }

        m_ParentSwapchain = swapchain;
        markAsInitialized();
        return true;
    }

    void VulkanSwapchainFramebuffer::clearFramebuffer()
    {
        vkDestroyFramebuffer(getRenderSubsystem()->getDevice(), m_Framebuffer, nullptr);
        m_Image.reset();

        m_Framebuffer = nullptr;
        m_ParentSwapchain = nullptr;
    }
}

#endif
