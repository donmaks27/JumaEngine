// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "VulkanFramebuffer.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "VulkanCommandBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"

namespace JumaEngine
{
    VulkanFramebuffer::~VulkanFramebuffer()
    {
        clearData();
    }

    void VulkanFramebuffer::clearData()
    {
        clearFramebuffer();

        if (m_ColorAttachmentImage != nullptr)
        {
            delete m_ColorAttachmentImage;
            m_ColorAttachmentImage = nullptr;
        }
        if (m_DepthAttachmentImage != nullptr)
        {
            delete m_DepthAttachmentImage;
            m_DepthAttachmentImage = nullptr;
        }
        if (m_ResolveAttachmentImage != nullptr)
        {
            delete m_ResolveAttachmentImage;
            m_ResolveAttachmentImage = nullptr;
        }

        m_RenderPass = nullptr;
        m_ExportAvailable = false;
    }
    void VulkanFramebuffer::clearFramebuffer()
    {
        if (m_Framebuffer != nullptr)
        {
            vkDestroyFramebuffer(getRenderSubsystemObject()->getDevice(), m_Framebuffer, nullptr);
            m_Framebuffer = nullptr;
        }
    }

    bool VulkanFramebuffer::update(const VulkanSwapchain* swapchain, const int8 swapchainImageIndex)
    {
        VkImage swapchainImage = swapchain != nullptr ? swapchain->getSwapchainImage(swapchainImageIndex) : nullptr;
        if (swapchainImage == nullptr)
        {
            JUMA_LOG(error, JSTR("Failed to get swapchain image"));
            return false;
        }

        VulkanRenderPass* renderPass = swapchain->getRenderPass();
        if ((renderPass == nullptr) || !renderPass->isValid() || !renderPass->getDescription().renderToSwapchain)
        {
            JUMA_LOG(error, JSTR("Invalid render pass"));
            return false;
        }

        const math::uvector2& size = swapchain->getSize();
        if ((size.x == 0) || (size.y == 0))
        {
            JUMA_LOG(error, JSTR("Invalid framebuffer size"));
            return false;
        }

        if (isValid())
        {
            VkImage resultImage = (m_ResolveAttachmentImage != nullptr ? m_ResolveAttachmentImage : m_ColorAttachmentImage)->get();
            if ((renderPass == getRenderPass()) && (size == getSize()) && (resultImage == swapchainImage))
            {
                return true;
            }
        }

        clearFramebuffer();
        return update(renderPass, size, swapchainImage);
    }
    bool VulkanFramebuffer::update(VulkanRenderPass* renderPass, const math::uvector2& size)
    {
        if ((renderPass == nullptr) || !renderPass->isValid() || renderPass->getDescription().renderToSwapchain)
        {
            JUMA_LOG(error, JSTR("Invalid render pass"));
            return false;
        }
        if ((size.x == 0) || (size.y == 0))
        {
            JUMA_LOG(error, JSTR("Invalid framebuffer size"));
            return false;
        }

        if (isValid() && (renderPass == getRenderPass()) && (size == getSize()))
        {
            return true;
        }
        return update(renderPass, size, nullptr);
    }

    bool VulkanFramebuffer::update(VulkanRenderPass* renderPass, const math::uvector2& size, VkImage resultImage)
    {
        m_RenderPass = renderPass;
        if (!recreateImages(size, resultImage) || !createFramebuffer(size))
        {
            JUMA_LOG(error, JSTR("Failed to initialize framebuffer"));
            clearData();
            return false;
        }

        markAsInitialized();
        return true;
    }
    bool VulkanFramebuffer::recreateImages(const math::uvector2& size, VkImage resultImage)
    {
        m_ExportAvailable = resultImage == nullptr;

        const VulkanRenderPassDescription& renderPassDescription = m_RenderPass->getDescription();
        const bool resolveImageEnabled = renderPassDescription.isResolveEnabled();

        if (m_ColorAttachmentImage == nullptr)
        {
            m_ColorAttachmentImage = getRenderSubsystemObject()->createVulkanObject<VulkanImage>();
        }
        else
        {
            m_ColorAttachmentImage->clear();
        }
        if (resolveImageEnabled)
        {
            m_ColorAttachmentImage->init(
                size, 1, renderPassDescription.sampleCount, renderPassDescription.colorFormat, 
                {}, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
                VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, 0
            );
            m_ColorAttachmentImage->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        }
        else if (resultImage != nullptr)
        {
            m_ColorAttachmentImage->init(resultImage, size, renderPassDescription.colorFormat);
            m_ColorAttachmentImage->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        }
        else
        {
            m_ColorAttachmentImage->init(
                size, 1, renderPassDescription.sampleCount, renderPassDescription.colorFormat,
                {}, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
                VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, 0
            );
            m_ColorAttachmentImage->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
            m_ColorAttachmentImage->createSampler();
        }

        if (renderPassDescription.shouldUseDepth)
        {
            if (m_DepthAttachmentImage == nullptr)
            {
                m_DepthAttachmentImage = getRenderSubsystemObject()->createVulkanObject<VulkanImage>();
            }
            else
            {
                m_DepthAttachmentImage->clear();
            }
            m_DepthAttachmentImage->init(
                size, 1, renderPassDescription.sampleCount, renderPassDescription.depthFormat,
                {}, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
                VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, 0
            );
            m_DepthAttachmentImage->createImageView(VK_IMAGE_ASPECT_DEPTH_BIT);
        }
        else if (m_DepthAttachmentImage != nullptr)
        {
            delete m_DepthAttachmentImage;
            m_DepthAttachmentImage = nullptr;
        }

        if (resolveImageEnabled)
        {
            if (m_ResolveAttachmentImage == nullptr)
            {
                m_ResolveAttachmentImage = getRenderSubsystemObject()->createVulkanObject<VulkanImage>();
            }
            else
            {
                m_ResolveAttachmentImage->clear();
            }
            if (resultImage != nullptr)
            {
                m_ResolveAttachmentImage->init(resultImage, size, renderPassDescription.colorFormat);
                m_ResolveAttachmentImage->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
            }
            else
            {
                m_ResolveAttachmentImage->init(
                    size, 1, VK_SAMPLE_COUNT_1_BIT, renderPassDescription.colorFormat,
                    {}, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, 0
                );
                m_ResolveAttachmentImage->createImageView(VK_IMAGE_ASPECT_COLOR_BIT);
                m_ResolveAttachmentImage->createSampler();
            }
        }
        else if (m_ResolveAttachmentImage != nullptr)
        {
            delete m_ResolveAttachmentImage;
            m_ResolveAttachmentImage = nullptr;
        }

        return true;
    }
    bool VulkanFramebuffer::createFramebuffer(const math::uvector2& size)
    {
        VkImageView attachments[3];
        uint32 attachmentCount = 1;
        attachments[0] = m_ColorAttachmentImage->getImageView();
        if (m_DepthAttachmentImage != nullptr)
        {
            attachments[attachmentCount++] = m_DepthAttachmentImage->getImageView();
        }
        if (m_ResolveAttachmentImage != nullptr)
        {
            attachments[attachmentCount++] = m_ResolveAttachmentImage->getImageView();
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass->get();
        framebufferInfo.attachmentCount = attachmentCount;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = size.x;
        framebufferInfo.height = size.y;
        framebufferInfo.layers = 1;
        const VkResult result = vkCreateFramebuffer(getRenderSubsystemObject()->getDevice(), &framebufferInfo, nullptr, &m_Framebuffer);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create framebuffer"), result);
            return false;
        }
        return true;
    }

    bool VulkanFramebuffer::startRender(VulkanCommandBuffer* commandBuffer)
    {
        if (!isValid() || (commandBuffer == nullptr) || !commandBuffer->isValid())
        {
            return false;
        }

        if (m_ExportAvailable)
        {
            VulkanImage* resultImage = getResultImage();
            resultImage->changeImageLayout(commandBuffer->get(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                0, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );
        }

        const math::uvector2 size = getSize();
        VkClearValue clearValues[2];
        clearValues[0].color = { { 1.0f, 1.0f, 1.0f, 1.0f } };
        clearValues[1].depthStencil = { 1.0f, 0 };
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_RenderPass->get();
        renderPassInfo.framebuffer = m_Framebuffer;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = { size.x, size.y };
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues;
        vkCmdBeginRenderPass(commandBuffer->get(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(size.x);
        viewport.height = static_cast<float>(size.y);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor;
        scissor.offset = { 0, 0 };
        scissor.extent = { size.x, size.y };
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

        if (m_ExportAvailable)
        {
            VulkanImage* resultImage = getResultImage();
            resultImage->changeImageLayout(commandBuffer->get(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            );
        }
        return true;
    }
}

#endif
