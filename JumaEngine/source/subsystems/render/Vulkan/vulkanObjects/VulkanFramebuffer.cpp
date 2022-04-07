// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "VulkanFramebuffer.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "VulkanRenderPass.h"
#include "VulkanImage.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"

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

        vkDestroyFramebuffer(getRenderSubsystemObject()->getDevice(), m_Framebuffer, nullptr);
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
        m_OutputColorImage = resultImage == nullptr;

        RenderSubsystem_RenderAPIObject_Vulkan* renderSubsystem = getRenderSubsystemObject();

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
        if (enableResolveImage)
        {
            m_ColorImage->init(
                m_ImagesSize, 1, renderPassDescription.sampleCount, renderPassDescription.colorFormat,
                {}, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, 0
            );
            m_ColorImage->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        }
        else if (resultImage == nullptr)
        {
            m_ColorImage->init(
                m_ImagesSize, 1, renderPassDescription.sampleCount, renderPassDescription.colorFormat,
                {}, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, 0
            );
            m_ColorImage->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
            m_ColorImage->createSampler();
        }
        else
        {
            m_ColorImage->init(resultImage, m_ImagesSize, renderPassDescription.colorFormat);
            m_ColorImage->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        }

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
                m_ResolveImage->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
            }
            else
            {
                m_ResolveImage->init(
                    m_ImagesSize, 1, VK_SAMPLE_COUNT_1_BIT, renderPassDescription.colorFormat,
                    {}, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, 0
                );
                m_ResolveImage->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
                m_ResolveImage->createSampler();
            }
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
        const VkResult result = vkCreateFramebuffer(getRenderSubsystemObject()->getDevice(), &framebufferInfo, nullptr, &m_Framebuffer);
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
            vkDestroyFramebuffer(getRenderSubsystemObject()->getDevice(), m_Framebuffer, nullptr);
            m_Framebuffer = nullptr;
        }
    }

    bool VulkanFramebuffer::startRender(VulkanCommandBuffer* commandBuffer)
    {
        if (!isValid() || (commandBuffer == nullptr) || !commandBuffer->isValid())
        {
            return false;
        }

        if (m_OutputColorImage)
        {
            VulkanImage* colorImage = m_ResolveImage != nullptr ? m_ResolveImage : m_ColorImage;
            colorImage->changeImageLayout(commandBuffer->get(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );
        }

        VkClearValue clearValues[2];
        clearValues[0].color = { { 1.0f, 1.0f, 1.0f, 1.0f } };
        clearValues[1].depthStencil = { 1.0f, 0 };
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_RenderPass->get();
        renderPassInfo.framebuffer = m_Framebuffer;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = { m_ImagesSize.x, m_ImagesSize.y };
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues;
        vkCmdBeginRenderPass(commandBuffer->get(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_ImagesSize.x);
        viewport.height = static_cast<float>(m_ImagesSize.y);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor;
        scissor.offset = { 0, 0 };
        scissor.extent = { m_ImagesSize.x, m_ImagesSize.y };
        vkCmdSetViewport(commandBuffer->get(), 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer->get(), 0, 1, &scissor);

        return true;
    }
    bool VulkanFramebuffer::finishRender(VulkanCommandBuffer* commandBuffer)
    {
        if (!isValid() || (commandBuffer == nullptr) || !commandBuffer->isValid())
        {
            return false;
        }

        vkCmdEndRenderPass(commandBuffer->get());

        if (m_OutputColorImage)
        {
            VulkanImage* colorImage = m_ResolveImage != nullptr ? m_ResolveImage : m_ColorImage;
            colorImage->changeImageLayout(commandBuffer->get(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );
        }

        return true;
    }
}

#endif
