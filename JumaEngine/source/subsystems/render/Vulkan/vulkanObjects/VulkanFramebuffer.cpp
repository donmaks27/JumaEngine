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

    bool VulkanFramebuffer::create(VulkanRenderPass* renderPass, const math::uvector2& size, const jshared_ptr<VulkanImage>& resultImage)
    {
        if ((renderPass == nullptr) || !renderPass->isValid())
        {
            JUMA_LOG(warning, JSTR("Invalid render pass"));
            return false;
        }
        if ((resultImage == nullptr) || !resultImage->isValid() || (resultImage->getSize() != size))
        {
            JUMA_LOG(warning, JSTR("Invalid result image"));
            return false;
        }

        return isValid() ? recreate(renderPass, size, resultImage) : init(renderPass, size, resultImage);
    }
    bool VulkanFramebuffer::create(VulkanRenderPass* renderPass, const math::uvector2& size, VkImage resultImage)
    {
        if (isValid() && (m_Images[m_ResultImageIndex]->get() == resultImage))
        {
            return create(renderPass, size, m_Images[m_ResultImageIndex]);
        }

        if ((renderPass == nullptr) || !renderPass->isValid())
        {
            JUMA_LOG(warning, JSTR("Invalid render pass"));
            return false;
        }
        if (resultImage == nullptr)
        {
            JUMA_LOG(warning, JSTR("Invalid result image"));
            return false;
        }

        VulkanImage* image = getRenderSubsystem()->createVulkanObject<VulkanImage>();
        image->init(resultImage, size, renderPass->getDescription().colorFormat, 1);
        return isValid() ? recreate(renderPass, size, image) : init(renderPass, size, image);
    }

    bool VulkanFramebuffer::init(VulkanRenderPass* renderPass, const math::uvector2& size, const jshared_ptr<VulkanImage>& resultImage)
    {
        m_RenderPass = renderPass;
        m_ImagesSize = size;
        if (!createImages(resultImage) || !createFrambuffer())
        {
            m_Images.clear();
            JUMA_LOG(error, JSTR("Failed to create framebuffer"));
            return false;
        }

        markAsInitialized();
        return true;
    }
    bool VulkanFramebuffer::recreate(VulkanRenderPass* renderPass, const math::uvector2& size, const jshared_ptr<VulkanImage>& resultImage)
    {
        vkDestroyFramebuffer(getRenderSubsystem()->getDevice(), m_Framebuffer, nullptr);
        if ((m_RenderPass != renderPass) || (m_ImagesSize != size))
        {
            m_RenderPass = renderPass;
            m_ImagesSize = size;
            m_Images.clear();
            if (!createImages(resultImage))
            {
                JUMA_LOG(error, JSTR("Failed to create framebuffer images"));
                return false;
            }
        }
        else
        {
            m_Images[m_ResultImageIndex] = resultImage;
        }

        if (!createFrambuffer())
        {
            m_Images.clear();
            JUMA_LOG(error, JSTR("Failed to create framebuffer"));
            return false;
        }
        return true;
    }

    bool VulkanFramebuffer::createImages(const jshared_ptr<VulkanImage>& resultImage)
    {
        m_ResultImageIndex = -1;
        m_Images.clear();

        const VulkanFramebufferImagesDescription imagesDescription = m_RenderPass->getImagesDescription();
        if (!imagesDescription.images.isValidIndex(imagesDescription.resultImageIndex))
        {
            JUMA_LOG(error, JSTR("Invalid images description"));
            return false;
        }

        m_Images.resize(imagesDescription.images.getSize());
        for (int32 index = 0; index < imagesDescription.images.getSize(); index++)
        {
            const VulkanFramebufferImageDescription& description = imagesDescription.images[index];
            jshared_ptr<VulkanImage>& image = m_Images[index];

            if (imagesDescription.resultImageIndex == index)
            {
                image = resultImage;
                m_ResultImageIndex = index;
            }
            else
            {
                image = getRenderSubsystem()->createVulkanObject<VulkanImage>();
            }

            if (!image->isValid())
            {
                image->init(
                    m_ImagesSize, 1, description.sampleCount, description.format, {}, 
                    (!description.depthImage ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, 
                    VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, 0
                );
                if (!image->isValid())
                {
                    JUMA_LOG(error, JSTR("Failed to create framebuffer image"));
                    return false;
                }
            }
            if (image->getImageView() == nullptr)
            {
                if (!image->createImageView(!description.depthImage ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT))
                {
                    JUMA_LOG(error, JSTR("Failed to create framebuffer image view"));
                    return false;
                }
            }
        }
        return m_Images.isValidIndex(m_ResultImageIndex);
    }
    bool VulkanFramebuffer::createFrambuffer()
    {
        jarray<VkImageView> attachments(m_Images.getSize());
        for (int32 index = 0; index < attachments.getSize(); index++)
        {
            attachments[index] = m_Images[index]->getImageView();
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
        if (m_Framebuffer != nullptr)
        {
            vkDestroyFramebuffer(getRenderSubsystem()->getDevice(), m_Framebuffer, nullptr);
            m_Images.clear();

            m_ResultImageIndex = -1;
            m_Framebuffer = nullptr;
            m_RenderPass = nullptr;
        }
    }
}

#endif
