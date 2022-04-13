// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderPipeline_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "RenderOptions_Vulkan.h"
#include "RenderSubsystem_Vulkan.h"
#include "RenderTarget_Vulkan.h"
#include "engine/Engine.h"
#include "subsystems/window/WindowSubsystem.h"
#include "subsystems/window/Vulkan/WindowSubsystem_Vulkan.h"
#include "vulkanObjects/VulkanCommandPool.h"
#include "vulkanObjects/VulkanSwapchain.h"

namespace JumaEngine
{
    RenderPipeline_RenderAPIObject_Vulkan::~RenderPipeline_RenderAPIObject_Vulkan()
    {
        clearData();
    }

    void RenderPipeline_RenderAPIObject_Vulkan::clearData()
    {
        VkDevice device = getRenderSubsystemObject()->getDevice();

        if (m_RenderFinishedFence != nullptr)
        {
            vkDestroyFence(device, m_RenderFinishedFence, nullptr);
            m_RenderFinishedFence = nullptr;
        }
        if (m_RenderFinishedSemaphore != nullptr)
        {
            vkDestroySemaphore(device, m_RenderFinishedSemaphore, nullptr);
            m_RenderFinishedFence = nullptr;
        }
        if (m_RenderCommandBuffer != nullptr)
        {
            m_RenderCommandBuffer->returnToCommandPool();
            m_RenderCommandBuffer = nullptr;
        }

        m_SwapchainImageReadySemaphores.clear();
    }

    bool RenderPipeline_RenderAPIObject_Vulkan::initInternal()
    {
        update();
        return true;
    }
    bool RenderPipeline_RenderAPIObject_Vulkan::onRenderPipelineUpdated()
    {
        return update();
    }
    bool RenderPipeline_RenderAPIObject_Vulkan::updateSyncObjects()
    {
        VkDevice device = getRenderSubsystemObject()->getDevice();
        if (m_RenderFinishedFence == nullptr)
        {
            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            const VkResult result = vkCreateFence(device, &fenceInfo, nullptr, &m_RenderFinishedFence);
            if (result != VK_SUCCESS)
            {
                JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create fence"), result);
                return false;
            }
        }
        if (m_RenderFinishedSemaphore == nullptr)
        {
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            const VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore);
            if (result != VK_SUCCESS)
            {
                JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create semaphore"), result);
                return false;
            }
        }
        return true;
    }

    bool RenderPipeline_RenderAPIObject_Vulkan::renderPipeline()
    {
        return waitForRenderFinish() && acquireNextSwapchainImages() && recordRenderCommandBuffer() && submitRenderCommandBuffer();
    }
    bool RenderPipeline_RenderAPIObject_Vulkan::waitForRenderFinish()
    {
        vkWaitForFences(getRenderSubsystemObject()->getDevice(), 1, &m_RenderFinishedFence, VK_TRUE, UINT64_MAX);
        if (m_RenderCommandBuffer != nullptr)
        {
            m_RenderCommandBuffer->returnToCommandPool();
            m_RenderCommandBuffer = nullptr;
        }
        return true;
    }
    bool RenderPipeline_RenderAPIObject_Vulkan::acquireNextSwapchainImages()
    {
        m_SwapchainImageReadySemaphores.clear();
        for (const auto& pipelineStage : m_Parent->getPipelineStages())
        {
            if (!pipelineStage.value.renderTarget->isWindowRenderTarget())
            {
                continue;
            }

            const window_id_type windowID = pipelineStage.value.renderTarget->getWindowID();
            const WindowSubsystem_RenderAPIObject_Vulkan* renderObject = m_Parent->getOwnerEngine()->getWindowSubsystem()->getRenderAPIObject<WindowSubsystem_RenderAPIObject_Vulkan>();
            VulkanSwapchain* swapchain = renderObject != nullptr ? renderObject->getVulkanSwapchain(windowID) : nullptr;
            if (swapchain == nullptr)
            {
                continue;
            }

            if (!swapchain->acquireNextImage())
            {
                return false;
            }
            m_SwapchainImageReadySemaphores.add(swapchain->getRenderAvailableSemaphore());
        }
        return true;
    }
    bool RenderPipeline_RenderAPIObject_Vulkan::recordRenderCommandBuffer()
    {
        if (!m_Parent->isPipelineQueueValid())
        {
            return false;
        }

        VulkanCommandPool* commandPool = getRenderSubsystemObject()->getCommandPool(VulkanQueueType::Graphics);
        VulkanCommandBuffer* commandBuffer = commandPool != nullptr ? commandPool->getCommandBuffer() : nullptr;
        if (commandBuffer == nullptr)
        {
            JUMA_LOG(error, JSTR("Failed to create render command buffer"));
            return false;
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;
        VkResult result = vkBeginCommandBuffer(commandBuffer->get(), &beginInfo);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to start render command buffer record"), result);
            commandBuffer->returnToCommandPool();
            return false;
        }

        RenderOptions_Vulkan renderOptions;
        renderOptions.renderPipeline = m_Parent;
        renderOptions.commandBuffer = commandBuffer;

        bool hasRenderCommands = false;
        for (const auto& stageName : m_Parent->getPipelineQueue())
        {
            const RenderPipelineStage* stage = m_Parent->getPipelineStage(stageName);
            const RenderTarget* renderTarget = stage != nullptr ? stage->renderTarget : nullptr;
            RenderTarget_RenderAPIObject_Vulkan* renderTargetObject = renderTarget != nullptr ? renderTarget->getRenderAPIObject<RenderTarget_RenderAPIObject_Vulkan>() : nullptr;
            if (renderTargetObject == nullptr)
            {
                continue;
            }

            if (!renderTargetObject->startRender(commandBuffer))
            {
                JUMA_LOG(error, JSTR("Failed to start render to \"") + stageName.toString() + JSTR("\" render image"));
                commandBuffer->returnToCommandPool();
                return false;
            }
            hasRenderCommands = true;

            renderOptions.renderTargetName = stageName;
            renderOptions.renderTarget = renderTarget;
            renderOptions.renderPass = renderTargetObject->getRenderPass();
            renderPipelineStage(&renderOptions);

            if (!renderTargetObject->finishRender(commandBuffer))
            {
                JUMA_LOG(error, JSTR("Failed to finish render to \"") + stageName.toString() + JSTR("\" render image"));
                commandBuffer->returnToCommandPool();
                return false;
            }
        }
        if (!hasRenderCommands)
        {
            commandBuffer->returnToCommandPool();
            return false;
        }

        result = vkEndCommandBuffer(commandBuffer->get());
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to finish render command buffer record"), result);
            commandBuffer->returnToCommandPool();
            return false;
        }

        m_RenderCommandBuffer = commandBuffer;
        return true;
    }
    bool RenderPipeline_RenderAPIObject_Vulkan::submitRenderCommandBuffer()
    {
        vkResetFences(getRenderSubsystemObject()->getDevice(), 1, &m_RenderFinishedFence);

        constexpr VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = m_SwapchainImageReadySemaphores.getSize();
        submitInfo.pWaitSemaphores = m_SwapchainImageReadySemaphores.getData();
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphore;
        if (!m_RenderCommandBuffer->submit(submitInfo, m_RenderFinishedFence, false))
        {
            JUMA_LOG(error, JSTR("Failed to submit render command buffer"));
            m_RenderCommandBuffer->returnToCommandPool();
            m_RenderCommandBuffer = nullptr;
            return false;
        }

        for (const auto& pipelineStage : m_Parent->getPipelineStages())
        {
            if (!pipelineStage.value.renderTarget->isWindowRenderTarget())
            {
                continue;
            }

            const window_id_type windowID = pipelineStage.value.renderTarget->getWindowID();
            const WindowSubsystem_RenderAPIObject_Vulkan* renderObject = m_Parent->getOwnerEngine()->getWindowSubsystem()->getRenderAPIObject<WindowSubsystem_RenderAPIObject_Vulkan>();
            VulkanSwapchain* swapchain = renderObject != nullptr ? renderObject->getVulkanSwapchain(windowID) : nullptr;
            if (swapchain == nullptr)
            {
                continue;
            }

            swapchain->presentCurrentImage(m_RenderFinishedSemaphore);
        }
        return true;
    }
}

#endif
