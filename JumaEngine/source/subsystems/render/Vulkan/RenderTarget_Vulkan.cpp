// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderTarget_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "RenderSubsystem_Vulkan.h"
#include "engine/Engine.h"
#include "subsystems/window/WindowSubsystem.h"
#include "subsystems/window/Vulkan/WindowSubsystem_Vulkan.h"
#include "vulkanObjects/VulkanCommandBuffer.h"
#include "vulkanObjects/VulkanFramebuffer.h"
#include "vulkanObjects/VulkanImage.h"
#include "vulkanObjects/VulkanRenderPassDescription.h"
#include "vulkanObjects/VulkanSwapchain.h"

namespace JumaEngine
{
    RenderTarget_RenderAPIObject_Vulkan::~RenderTarget_RenderAPIObject_Vulkan()
    {
        clearData();
    }

    void RenderTarget_RenderAPIObject_Vulkan::clearData()
    {
        for (const auto& framebuffer : m_Framebuffers)
        {
            delete framebuffer;
        }
        m_Framebuffers.clear();
        m_RenderPass = nullptr;
        m_Swapchain = nullptr;
    }

    bool RenderTarget_RenderAPIObject_Vulkan::initInternal()
    {
        const bool initialized = m_Parent->isWindowRenderTarget() ? initWindowRenderTarget() : initRenderTarget();
        if (!initialized || !updateFramebuffers())
        {
            JUMA_LOG(error, JSTR("Failed to initialize render target Vulkan API object"));
            return false;
        }
        return true;
    }
    bool RenderTarget_RenderAPIObject_Vulkan::initWindowRenderTarget()
    {
        const WindowSubsystem_RenderAPIObject_Vulkan* windowSubsystemObject = m_Parent->getOwnerEngine()->getWindowSubsystem()->getRenderAPIObject<WindowSubsystem_RenderAPIObject_Vulkan>();
        VulkanSwapchain* swapchain = windowSubsystemObject != nullptr ? windowSubsystemObject->getVulkanSwapchain(m_Parent->getWindowID()) : nullptr;
        if ((swapchain == nullptr) || !swapchain->isValid())
        {
            JUMA_LOG(error, JSTR("Invalid window's swapchain"));
            return false;
        }

        VulkanRenderPassDescription renderPassDescription;
        renderPassDescription.colorFormat = swapchain->getFormat();
        renderPassDescription.depthFormat = VK_FORMAT_D32_SFLOAT;
        renderPassDescription.sampleCount = GetSampleCountByTextureSamples(m_Parent->getTextureSamples());
        renderPassDescription.shouldUseDepth = true;
        renderPassDescription.renderToSwapchain = true;
        VulkanRenderPass* renderPass = getRenderSubsystemObject()->getRenderPass(renderPassDescription);
        if (renderPass == nullptr)
        {
            JUMA_LOG(error, JSTR("Failed to get render pass"));
            return false;
        }

        m_Swapchain = swapchain;
        m_RenderPass = renderPass;
        return true;
    }
    bool RenderTarget_RenderAPIObject_Vulkan::initRenderTarget()
    {
        const TextureFormat format = m_Parent->getFormat();
        const VkFormat vulkanFormat = GetVulkanFormatByTextureFormat(format);
        if (vulkanFormat == VK_FORMAT_UNDEFINED)
        {
            JUMA_LOG(error, JSTR("Invalid render target's format"));
            return false;
        }

        VulkanRenderPassDescription renderPassDescription;
        renderPassDescription.colorFormat = vulkanFormat;
        renderPassDescription.depthFormat = VK_FORMAT_D32_SFLOAT;
        renderPassDescription.sampleCount = GetSampleCountByTextureSamples(m_Parent->getTextureSamples());
        renderPassDescription.shouldUseDepth = true;
        renderPassDescription.renderToSwapchain = false;
        VulkanRenderPass* renderPass = getRenderSubsystemObject()->getRenderPass(renderPassDescription);
        if (renderPass == nullptr)
        {
            JUMA_LOG(error, JSTR("Failed to get render pass"));
            return false;
        }

        m_Swapchain = nullptr;
        m_RenderPass = renderPass;
        return true;
    }

    bool RenderTarget_RenderAPIObject_Vulkan::updateFramebuffers()
    {
        const int8 framebufferCount = getFramebufferCount();
        if (framebufferCount <= 0)
        {
            JUMA_LOG(error, JSTR("Failed to get framebuffer count"));
            return false;
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

            const bool updated = m_Swapchain != nullptr ? framebuffer->update(m_RenderPass, m_Swapchain, index) : framebuffer->update(m_RenderPass, m_Parent->getSize());
            if (!updated)
            {
                JUMA_LOG(error, JSTR("Failed to update framebuffer ") + TO_JSTR(index));
                return false;
            }
        }
        return true;
    }
    int8 RenderTarget_RenderAPIObject_Vulkan::getFramebufferCount() const
    {
        return m_Swapchain != nullptr ? static_cast<int8>(m_Swapchain->getImageCount()) : getRenderFrameCount();
    }
    int8 RenderTarget_RenderAPIObject_Vulkan::getRenderFrameCount() const
    {
        return getRenderSubsystemObject()->getRenderFrameCount();
    }
    int8 RenderTarget_RenderAPIObject_Vulkan::getCurrentFramebufferIndex() const
    {
        return m_Swapchain != nullptr ? m_Swapchain->getSwapchainImageIndex() : getCurrentRenderFrameIndex();
    }
    int8 RenderTarget_RenderAPIObject_Vulkan::getCurrentRenderFrameIndex() const
    {
        return getRenderSubsystemObject()->getRenderFrameIndex();
    }

    bool RenderTarget_RenderAPIObject_Vulkan::startRender(VulkanCommandBuffer* commandBuffer)
    {
        if ((commandBuffer == nullptr) || !commandBuffer->isValid())
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
    bool RenderTarget_RenderAPIObject_Vulkan::finishRender(VulkanCommandBuffer* commandBuffer)
    {
        if ((commandBuffer == nullptr) || !commandBuffer->isValid())
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
