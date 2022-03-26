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

        m_CurrentFramebufferIndex = -1;
        m_CurrentRenderFrameIndex = -1;
    }

    int8 VulkanRenderImage::getDesiredRenderFrameCount() const
    {
        if (!isValid())
        {
            return 0;
        }

        // TODO: Get it from render subsystem
        return 2;
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
        const int8 count = getDesiredRenderFrameCount();
        if (count <= 0)
        {
            JUMA_LOG(error, JSTR("Failed to get desired render frame count"));
            return false;
        }

        if (count == m_RenderFrames.getSize())
        {
            return true;
        }

        VkDevice device = getRenderSubsystem()->getDevice();
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (int32 index = m_RenderFrames.getSize() - 1; index >= count; index--)
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
        m_RenderFrames.resize(count);
        for (int32 index = 0; index < count; index++)
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
        const int8 count = m_Swapchain != nullptr ? static_cast<int8>(m_Swapchain->getImageCount()) : getDesiredRenderFrameCount();
        if (count <= 0)
        {
            JUMA_LOG(error, JSTR("Failed to get desired framebuffer count"));
            return false;
        }

        if (m_Swapchain != nullptr)
        {
            m_RenderPass = m_Swapchain->getRenderPass();
            m_FramebufferSize = m_Swapchain->getSize();
        }

        for (int32 index = m_Framebuffers.getSize() - 1; index >= count; index--)
        {
            delete m_Framebuffers[index];
        }
        m_Framebuffers.resize(count);
        for (int32 index = 0; index < count; index++)
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

    bool VulkanRenderImage::setRenderFrameIndex(const int8 renderFrameIndex)
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Render image not initialized"));
            return false;
        }
        if (!m_RenderFrames.isValidIndex(renderFrameIndex))
        {
            JUMA_LOG(error, JSTR("Invalid render frame index"));
            return false;
        }

        const int8 framebufferIndex = m_Swapchain != nullptr ? m_Swapchain->getSwapchainImageIndex() : renderFrameIndex;
        if (!m_Framebuffers.isValidIndex(framebufferIndex))
        {
            JUMA_LOG(error, JSTR("Invalid framebuffer index"));
            return false;
        }

        m_CurrentRenderFrameIndex = renderFrameIndex;
        m_CurrentFramebufferIndex = framebufferIndex;
        return true;
    }

    VulkanCommandBuffer* VulkanRenderImage::startRenderCommandBufferRecord()
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Render image not initialized"));
            return nullptr;
        }
        if (!m_Framebuffers.isValidIndex(m_CurrentFramebufferIndex) || !m_RenderFrames.isValidIndex(m_CurrentRenderFrameIndex))
        {
            JUMA_LOG(error, JSTR("Invalid render image state"));
            return nullptr;
        }

        VulkanCommandBuffer* commandBuffer = m_Framebuffers[m_CurrentFramebufferIndex]->createRenderCommandBuffer();
        if (commandBuffer == nullptr)
        {
            JUMA_LOG(error, JSTR("Failed to create render command buffer"));
            return nullptr;
        }

        RenderFrame& frame = m_RenderFrames[m_CurrentRenderFrameIndex];
        if (frame.commandBuffer != nullptr)
        {
            frame.commandBuffer->returnToCommandPool();
        }
        frame.commandBuffer = commandBuffer;
        vkResetFences(getRenderSubsystem()->getDevice(), 1, &frame.finishFence);

        return commandBuffer;
    }
    bool VulkanRenderImage::submitRenderCommandBuffer()
    {
        if (!isValid())
        {
            JUMA_LOG(error, JSTR("Render image not initialized"));
            return false;
        }
        if (!m_RenderFrames.isValidIndex(m_CurrentRenderFrameIndex))
        {
            JUMA_LOG(error, JSTR("Invalid render image state"));
            return false;
        }

        RenderFrame& frame = m_RenderFrames[m_CurrentRenderFrameIndex];
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
