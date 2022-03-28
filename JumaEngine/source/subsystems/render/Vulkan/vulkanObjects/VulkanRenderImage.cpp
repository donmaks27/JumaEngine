// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "VulkanRenderImage.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "VulkanCommandBuffer.h"
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
        VkDevice device = getRenderSubsystem()->getDevice();

        for (const auto& framebuffer : m_Framebuffers)
        {
            delete framebuffer;
        }
        m_Framebuffers.clear();

        for (const auto& frame : m_RenderFrames)
        {
            if (frame.commandBuffer != nullptr)
            {
                frame.commandBuffer->returnToCommandPool();
            }
            if (frame.finishFence != nullptr)
            {
                vkDestroyFence(device, frame.finishFence, nullptr);
            }
            if (frame.finishSemaphore != nullptr)
            {
                vkDestroySemaphore(device, frame.finishSemaphore, nullptr);
            }
        }
        m_RenderFrames.clear();

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

    bool VulkanRenderImage::update()
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Render image not initialized"));
            return false;
        }

        if (!updateRenderFrameCount() || !updateFramebufferCount() || !updateDependencies())
        {
            JUMA_LOG(error, JSTR("Failed to update render image"));
            return false;
        }
        return true;
    }
    bool VulkanRenderImage::updateRenderFrameCount()
    {
        const int8 frameCount = getRenderSubsystem()->getRenderFrameCount();
        if (frameCount <= 0)
        {
            JUMA_LOG(error, JSTR("Failed to get render frame count"));
            return false;
        }

        if (frameCount == m_RenderFrames.getSize())
        {
            return true;
        }

        VkDevice device = getRenderSubsystem()->getDevice();
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (int32 index = m_RenderFrames.getSize() - 1; index >= frameCount; index--)
        {
            RenderFrame& frameData = m_RenderFrames[index];
            if (frameData.finishFence != nullptr)
            {
                vkDestroyFence(device, frameData.finishFence, nullptr);
            }
            if (frameData.finishSemaphore != nullptr)
            {
                vkDestroySemaphore(device, frameData.finishSemaphore, nullptr);
            }
        }
        m_RenderFrames.resize(frameCount);
        for (int32 index = 0; index < frameCount; index++)
        {
            RenderFrame& frameData = m_RenderFrames[index];
            if (frameData.finishFence == nullptr)
            {
                const VkResult result = vkCreateFence(device, &fenceInfo, nullptr, &frameData.finishFence);
                if (result != VK_SUCCESS)
                {
                    JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create fence ") + TO_JSTR(index), result);
                    return false;
                }
            }
            if (frameData.finishSemaphore == nullptr)
            {
                const VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &frameData.finishSemaphore);
                if (result != VK_SUCCESS)
                {
                    JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create semaphore ") + TO_JSTR(index), result);
                    return false;
                }
            }
        }
        return true;
    }
    bool VulkanRenderImage::updateFramebufferCount()
    {
        const int8 framebufferCount = m_Swapchain != nullptr ? static_cast<int8>(m_Swapchain->getImageCount()) : getRenderSubsystem()->getRenderFrameCount();
        if (framebufferCount <= 0)
        {
            JUMA_LOG(error, JSTR("Failed to get desired framebuffer count"));
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
        for (int32 index = 0; index < framebufferCount; index++)
        {
            VulkanFramebuffer*& framebuffer = m_Framebuffers[index];
            if (framebuffer == nullptr)
            {
                framebuffer = getRenderSubsystem()->createVulkanObject<VulkanFramebuffer>();
            }
            if (!framebuffer->create(m_RenderPass, m_FramebufferSize, m_Swapchain != nullptr ? m_Swapchain->getSwapchainImage(index) : nullptr))
            {
                JUMA_LOG(error, JSTR("Failed to update framebuffer ") + TO_JSTR(index));
                return false;
            }
        }
        return true;
    }
    bool VulkanRenderImage::updateDependencies()
    {
        for (int32 index = 0; index < m_RenderFrames.getSize(); index++)
        {
            if (m_Swapchain != nullptr)
            {
                m_RenderFrames[index].startSemaphores = { m_Swapchain->getRenderFrameAvailableSemaphore(index) };
            }
            else
            {
                m_RenderFrames[index].startSemaphores.clear();
            }
        }
        return true;
    }

    int8 VulkanRenderImage::getCurrentFramebufferIndex() const
    {
        return m_Swapchain != nullptr ? m_Swapchain->getSwapchainImageIndex() : getCurrentRenderFrameIndex();
    }
    int8 VulkanRenderImage::getCurrentRenderFrameIndex() const
    {
        return getRenderSubsystem()->getRenderFrameIndex();
    }

    VkFence VulkanRenderImage::getFinishFence() const
    {
        const int8 renderFrameIndex = getCurrentRenderFrameIndex();
        return m_RenderFrames.isValidIndex(renderFrameIndex) ? m_RenderFrames[renderFrameIndex].finishFence : nullptr;
    }
    VkSemaphore VulkanRenderImage::getFinishSemaphore() const
    {
        const int8 renderFrameIndex = getCurrentRenderFrameIndex();
        return m_RenderFrames.isValidIndex(renderFrameIndex) ? m_RenderFrames[renderFrameIndex].finishSemaphore : nullptr;
    }

    VulkanCommandBuffer* VulkanRenderImage::startRenderCommandBufferRecord()
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Render image not initialized"));
            return nullptr;
        }
        const int8 framebufferIndex = getCurrentFramebufferIndex();
        const int8 frameIndex = getCurrentRenderFrameIndex();
        if (!m_Framebuffers.isValidIndex(framebufferIndex) || !m_RenderFrames.isValidIndex(frameIndex))
        {
            JUMA_LOG(error, JSTR("Invalid render image state"));
            return nullptr;
        }

        VulkanCommandBuffer* commandBuffer = m_Framebuffers[framebufferIndex]->createRenderCommandBuffer();
        if (commandBuffer == nullptr)
        {
            JUMA_LOG(error, JSTR("Failed to create render command buffer"));
            return nullptr;
        }

        RenderFrame& frame = m_RenderFrames[frameIndex];
        if (frame.commandBuffer != nullptr)
        {
            frame.commandBuffer->returnToCommandPool();
        }
        frame.commandBuffer = commandBuffer;
        vkResetFences(getRenderSubsystem()->getDevice(), 1, &frame.finishFence);

        return commandBuffer;
    }
    VulkanCommandBuffer* VulkanRenderImage::getRenderCommandBuffer() const
    {
        const int8 renderFrameIndex = getCurrentRenderFrameIndex();
        return m_RenderFrames.isValidIndex(renderFrameIndex) ? m_RenderFrames[renderFrameIndex].commandBuffer : nullptr;
    }
    bool VulkanRenderImage::submitRenderCommandBuffer()
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Render image not initialized"));
            return false;
        }
        const int8 frameIndex = getCurrentRenderFrameIndex();
        if (!m_RenderFrames.isValidIndex(frameIndex))
        {
            JUMA_LOG(error, JSTR("Invalid render image state"));
            return false;
        }

        RenderFrame& frame = m_RenderFrames[frameIndex];
        if (frame.commandBuffer == nullptr)
        {
            JUMA_LOG(error, JSTR("Empty render command buffer"));
            return false;
        }

        vkCmdEndRenderPass(frame.commandBuffer->get());
        const VkResult result = vkEndCommandBuffer(frame.commandBuffer->get());
        if (result != VK_SUCCESS)
        {
            frame.commandBuffer->returnToCommandPool();
            frame.commandBuffer = nullptr;

            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to finish record to command buffer"), result);
            return false;
        }

        constexpr VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = frame.startSemaphores.getSize();
        submitInfo.pWaitSemaphores = frame.startSemaphores.getData();
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &frame.finishSemaphore;
        if (!frame.commandBuffer->submit(submitInfo, frame.finishFence, false))
        {
            frame.commandBuffer->returnToCommandPool();
            frame.commandBuffer = nullptr;

            JUMA_LOG(error, JSTR("Failed to submit render command buffer"));
            return false;
        }
        return true;
    }
}

#endif
