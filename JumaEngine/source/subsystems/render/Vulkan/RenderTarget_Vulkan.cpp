// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderTarget_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "RenderSubsystem_Vulkan.h"
#include "vulkanObjects/VulkanFramebuffer.h"
#include "vulkanObjects/VulkanRenderPass.h"

namespace JumaEngine
{
    bool RenderTarget_Vulkan::createFramebuffers(const uint8 framebufferCount, VulkanRenderPass* renderPass, const math::uvector2& size)
    {
        if ((framebufferCount == 0) || (renderPass == nullptr) || !renderPass->isValid() || (size.x == 0) || (size.y == 0))
        {
            JUMA_LOG(error, JSTR("Invalid input params"));
            return false;
        }

        for (int32 index = m_Framebuffers.getSize() - 1; index >= framebufferCount; index--)
        {
            delete m_Framebuffers[index].framebuffer;
        }

        m_RenderPass = renderPass;
        m_FramebufferSize = size;
        m_Framebuffers.resize(framebufferCount);
        for (auto& framebuffer : m_Framebuffers)
        {
            if (framebuffer.framebuffer == nullptr)
            {
                framebuffer.framebuffer = getRenderSubsystem()->createVulkanObject<VulkanFramebuffer>();
            }
        }
        return true;
    }
    bool RenderTarget_Vulkan::createRenderFrames(const uint8 frameCount)
    {
        VkDevice device = getRenderSubsystem()->getDevice();
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (int32 index = m_RenderFrames.getSize() - 1; index >= frameCount; index--)
        {
            const RenderFrame& frame = m_RenderFrames[index];
            if (frame.renderFinishedFence != nullptr)
            {
                vkDestroyFence(device, frame.renderFinishedFence, nullptr);
            }
            if (frame.renderFinishedSemaphore != nullptr)
            {
                vkDestroySemaphore(device, frame.renderFinishedSemaphore, nullptr);
            }
        }
        m_RenderFrames.resize(frameCount);
        for (auto& frame : m_RenderFrames)
        {
            if (frame.renderFinishedFence == nullptr)
            {
                const VkResult result = vkCreateFence(getRenderSubsystem()->getDevice(), &fenceInfo, nullptr, &frame.renderFinishedFence);
                if (result != VK_SUCCESS)
                {
                    JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create fence"), result);
                    return false;
                }
            }
            if (frame.renderFinishedSemaphore == nullptr)
            {
                const VkResult result = vkCreateSemaphore(getRenderSubsystem()->getDevice(), &semaphoreInfo, nullptr, &frame.renderFinishedSemaphore);
                if (result != VK_SUCCESS)
                {
                    JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create semaphore"), result);
                    return false;
                }
            }
        }
        return true;
    }
}

#endif
