// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "VulkanRenderImage.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "VulkanCommandPool.h"
#include "VulkanFramebuffer.h"
#include "VulkanSwapchain.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"

namespace JumaEngine
{
    VulkanRenderImage::~VulkanRenderImage()
    {
        clearVulkan();
    }

    void VulkanRenderImage::clearVulkan()
    {
        for (const auto& framebuffer : m_Framebuffers)
        {
            delete framebuffer;
        }
        m_Framebuffers.clear();

        m_Swapchain = nullptr;
        m_RenderPass = nullptr;
        m_FramebufferSize = { 0, 0 };
    }

    bool VulkanRenderImage::init(VulkanSwapchain* swapchain)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Render image already initialized"));
            return false;
        }
        if (swapchain == nullptr)
        {
            JUMA_LOG(error, JSTR("Invalid swapchain"));
            return false;
        }

        m_Swapchain = swapchain;
        markAsInitialized();
        return true;
    }
    bool VulkanRenderImage::init(VulkanRenderPass* renderPass, const math::uvector2& framebufferSize)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Render image already initialized"));
            return false;
        }
        if ((renderPass == nullptr) || (framebufferSize.x == 0) || (framebufferSize.y == 0))
        {
            JUMA_LOG(error, JSTR("Invalid input params"));
            return false;
        }

        m_RenderPass = renderPass;
        m_FramebufferSize = framebufferSize;
        markAsInitialized();
        return true;
    }

    int8 VulkanRenderImage::getFramebufferCount() const
    {
        return m_Swapchain != nullptr ? static_cast<int8>(m_Swapchain->getImageCount()) : getRenderFrameCount();
    }
    int8 VulkanRenderImage::getRenderFrameCount() const
    {
        return getRenderSubsystemObject()->getRenderFrameCount();
    }
    int8 VulkanRenderImage::getCurrentFramebufferIndex() const
    {
        return m_Swapchain != nullptr ? m_Swapchain->getSwapchainImageIndex() : getCurrentRenderFrameIndex();
    }
    int8 VulkanRenderImage::getCurrentRenderFrameIndex() const
    {
        return getRenderSubsystemObject()->getRenderFrameIndex();
    }

    bool VulkanRenderImage::update()
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Render image not initialized"));
            return false;
        }

        if (!updateFramebufferCount())
        {
            JUMA_LOG(error, JSTR("Failed to update render image"));
            return false;
        }
        return true;
    }
    bool VulkanRenderImage::updateFramebufferCount()
    {
        const int8 framebufferCount = getFramebufferCount();
        if (framebufferCount <= 0)
        {
            JUMA_LOG(error, JSTR("Failed to get framebuffer count"));
            return false;
        }

        if (m_Swapchain != nullptr)
        {
            m_RenderPass = m_Swapchain->getRenderPass();
            m_FramebufferSize = m_Swapchain->getSize();
        }

        for (int32 index = m_Framebuffers.getSize() - 1; index >= framebufferCount; index--)
        {
            delete m_Framebuffers[index];
        }
        m_Framebuffers.resize(framebufferCount);
        for (int8 index = 0; index < framebufferCount; index++)
        {
            VulkanFramebuffer*& framebuffer = m_Framebuffers[index];
            if (framebuffer == nullptr)
            {
                framebuffer = getRenderSubsystemObject()->createVulkanObject<VulkanFramebuffer>();
            }

            const bool updated = m_Swapchain != nullptr ? framebuffer->update(m_Swapchain, index) : framebuffer->update(m_RenderPass, m_FramebufferSize);
            if (!updated)
            {
                JUMA_LOG(error, JSTR("Failed to update framebuffer ") + TO_JSTR(index));
                return false;
            }
        }
        return true;
    }

    bool VulkanRenderImage::startRender(VulkanCommandBuffer* commandBuffer)
    {
        if (!isValid() || (commandBuffer == nullptr) || !commandBuffer->isValid())
        {
            return false;
        }

        const int8 framebufferIndex = getCurrentFramebufferIndex();
        if (!m_Framebuffers.isValidIndex(framebufferIndex))
        {
            JUMA_LOG(error, JSTR("Invalid framebuffer index"));
            return false;
        }
        return m_Framebuffers[framebufferIndex]->startRender(commandBuffer);
    }
    bool VulkanRenderImage::finishRender(VulkanCommandBuffer* commandBuffer)
    {
        if (!isValid() || (commandBuffer == nullptr) || !commandBuffer->isValid())
        {
            return false;
        }

        const int8 framebufferIndex = getCurrentFramebufferIndex();
        if (!m_Framebuffers.isValidIndex(framebufferIndex))
        {
            JUMA_LOG(error, JSTR("Invalid framebuffer index"));
            return false;
        }
        return m_Framebuffers[framebufferIndex]->finishRender(commandBuffer);
    }
}

#endif
