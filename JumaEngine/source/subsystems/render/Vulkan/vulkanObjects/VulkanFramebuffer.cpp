// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "VulkanFramebuffer.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "VulkanRenderPass.h"
#include "VulkanImage.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"

namespace JumaEngine
{
    VulkanFramebuffer::~VulkanFramebuffer()
    {
        clearFramebuffer();
    }

    bool VulkanFramebuffer::create(VulkanRenderPass* renderPass, const math::uvector2& size, VkImage resultImage)
    {
        if ((renderPass == nullptr) || !renderPass->isValid())
        {
            JUMA_LOG(warning, JSTR("Invalid render pass"));
            return false;
        }
        return isValid() ? recreate(renderPass, size, resultImage) : init(renderPass, size, resultImage);
    }

    bool VulkanFramebuffer::init(VulkanRenderPass* renderPass, const math::uvector2& size, VkImage resultImage)
    {
        m_RenderPass = renderPass;
        m_ImagesSize = size;
        if (!recreateImages(resultImage) || !createFrambuffer())
        {
            JUMA_LOG(error, JSTR("Failed to create framebuffer"));
            clear();
            return false;
        }

        markAsInitialized();
        return true;
    }
    bool VulkanFramebuffer::recreate(VulkanRenderPass* renderPass, const math::uvector2& size, VkImage resultImage)
    {
        if ((m_RenderPass != renderPass) || (m_ImagesSize != size))
        {
            clear();
            return init(renderPass, size, resultImage);
        }

        vkDestroyFramebuffer(getRenderSubsystem()->getDevice(), m_Framebuffer, nullptr);
        VulkanImage* image = m_ResolveImage != nullptr ? m_ResolveImage : m_ColorImage;
        image->clear();

        const VkFormat colorFormat = renderPass->getDescription().colorFormat;
        if (resultImage != nullptr)
        {
            image->init(resultImage, m_ImagesSize, colorFormat);
        }
        else
        {
            image->init(
                m_ImagesSize, 1, VK_SAMPLE_COUNT_1_BIT, colorFormat,
                {}, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
                VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, 0
            );
        }
        image->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);

        if (!createFrambuffer())
        {
            JUMA_LOG(error, JSTR("Failed to create framebuffer"));
            clear();
            return false;
        }
        return true;
    }

    bool VulkanFramebuffer::recreateImages(VkImage resultImage)
    {
        RenderSubsystem_Vulkan* renderSubsystem = getRenderSubsystem();

        const VulkanRenderPassDescription& renderPassDescription = m_RenderPass->getDescription();
        const bool enableResolveImage = renderPassDescription.sampleCount != VK_SAMPLE_COUNT_1_BIT;
        
        if (m_ColorImage == nullptr)
        {
            m_ColorImage = renderSubsystem->createVulkanObject<VulkanImage>();
        }
        else
        {
            m_ColorImage->clear();
        }
        if (enableResolveImage || (resultImage == nullptr))
        {
            m_ColorImage->init(
                m_ImagesSize, 1, renderPassDescription.sampleCount, renderPassDescription.colorFormat,
                {}, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
                VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, 0
            );
        }
        else
        {
            m_ColorImage->init(resultImage, m_ImagesSize, renderPassDescription.colorFormat);
        }
        m_ColorImage->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);

        if (m_DepthImage == nullptr)
        {
            m_DepthImage = renderSubsystem->createVulkanObject<VulkanImage>();
        }
        else
        {
            m_DepthImage->clear();
        }
        m_DepthImage->init(
            m_ImagesSize, 1, renderPassDescription.sampleCount, renderPassDescription.depthFormat,
            {}, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
            VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, 0
        );
        m_DepthImage->createImageView(VK_IMAGE_ASPECT_DEPTH_BIT);

        if (enableResolveImage)
        {
            if (m_ResolveImage == nullptr)
            {
                m_ResolveImage = renderSubsystem->createVulkanObject<VulkanImage>();
            }
            else
            {
                m_ResolveImage->clear();
            }
            if (resultImage != nullptr)
            {
                m_ResolveImage->init(resultImage, m_ImagesSize, renderPassDescription.colorFormat);
            }
            else
            {
                m_ResolveImage->init(
                    m_ImagesSize, 1, VK_SAMPLE_COUNT_1_BIT, renderPassDescription.colorFormat,
                    {}, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
                    VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, 0
                );
            }
            m_ResolveImage->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        }
        else if (m_ResolveImage != nullptr)
        {
            delete m_ResolveImage;
            m_ResolveImage = nullptr;
        }

        return true;
    }
    bool VulkanFramebuffer::createFrambuffer()
    {
        jarray<VkImageView> attachments(m_ResolveImage != nullptr ? 3 : 2);
        attachments[0] = m_ColorImage->getImageView();
        attachments[1] = m_DepthImage->getImageView();
        if (m_ResolveImage != nullptr)
        {
            attachments[2] = m_ResolveImage->getImageView();
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass->get();
        framebufferInfo.attachmentCount = static_cast<uint32>(attachments.getSize());
        framebufferInfo.pAttachments = attachments.getData();
        framebufferInfo.width = m_ImagesSize.x;
        framebufferInfo.height = m_ImagesSize.y;
        framebufferInfo.layers = 1;
        const VkResult result = vkCreateFramebuffer(getRenderSubsystem()->getDevice(), &framebufferInfo, nullptr, &m_Framebuffer);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create framebuffer"), result);
            return false;
        }
        return true;
    }

    void VulkanFramebuffer::clearFramebuffer()
    {
        m_RenderPass = nullptr;
        m_ImagesSize = { 0, 0 };

        if (m_ColorImage != nullptr)
        {
            delete m_ColorImage;
            m_ColorImage = nullptr;
        }
        if (m_DepthImage != nullptr)
        {
            delete m_DepthImage;
            m_DepthImage = nullptr;
        }
        if (m_ResolveImage != nullptr)
        {
            delete m_ResolveImage;
            m_ResolveImage = nullptr;
        }

        if (m_Framebuffer != nullptr)
        {
            vkDestroyFramebuffer(getRenderSubsystem()->getDevice(), m_Framebuffer, nullptr);
            m_Framebuffer = nullptr;
        }
    }
}

#endif
