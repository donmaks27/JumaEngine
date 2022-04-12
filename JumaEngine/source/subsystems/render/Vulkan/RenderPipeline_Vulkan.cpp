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

        for (const auto& renderFrameObjects : m_RenderFramesObjects)
        {
            if (renderFrameObjects.commandBuffer != nullptr)
            {
                renderFrameObjects.commandBuffer->returnToCommandPool();
            }
            if (renderFrameObjects.renderFinishedSemaphore != nullptr)
            {
                vkDestroySemaphore(device, renderFrameObjects.renderFinishedSemaphore, nullptr);
            }
            if (renderFrameObjects.renderFinishedFence != nullptr)
            {
                vkDestroyFence(device, renderFrameObjects.renderFinishedFence, nullptr);
            }
        }
        m_RenderFramesObjects.clear();

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
        const int8 renderFrameCount = getRenderSubsystemObject()->getRenderFrameCount();
        if (renderFrameCount <= 0)
        {
            JUMA_LOG(error, JSTR("Invalid render frame count"));
            return false;
        }
        if (m_RenderFramesObjects.getSize() == renderFrameCount)
        {
            return true;
        }

        VkDevice device = getRenderSubsystemObject()->getDevice();
        for (int32 index = m_RenderFramesObjects.getSize() - 1; index >= renderFrameCount; index--)
        {
            RenderFrameObjects& objects = m_RenderFramesObjects[index];
            vkDestroyFence(device, objects.renderFinishedFence, nullptr);
            vkDestroySemaphore(device, objects.renderFinishedSemaphore, nullptr);
        }
        m_RenderFramesObjects.resize(renderFrameCount);

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        for (int32 index = 0; index < m_RenderFramesObjects.getSize(); index++)
        {
            RenderFrameObjects& objects = m_RenderFramesObjects[index];
            if (objects.renderFinishedFence == nullptr)
            {
                const VkResult result = vkCreateFence(device, &fenceInfo, nullptr, &objects.renderFinishedFence);
                if (result != VK_SUCCESS)
                {
                    JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create fence"), result);
                    return false;
                }
            }
            if (objects.renderFinishedSemaphore == nullptr)
            {
                const VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &objects.renderFinishedSemaphore);
                if (result != VK_SUCCESS)
                {
                    JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create semaphore"), result);
                    return false;
                }
            }
        }
        return true;
    }

    bool RenderPipeline_RenderAPIObject_Vulkan::renderPipeline()
    {
        getRenderSubsystemObject()->updateRenderFrameIndex();
        return waitForRenderFinish() && acquireNextSwapchainImages() && recordRenderCommandBuffer() && submitRenderCommandBuffer();
    }
    bool RenderPipeline_RenderAPIObject_Vulkan::waitForRenderFinish()
    {
        const int8 renderFrameIndex = getRenderSubsystemObject()->getRenderFrameIndex();
        if (!m_RenderFramesObjects.isValidIndex(renderFrameIndex))
        {
            return false;
        }

        RenderFrameObjects& renderFrameObjects = m_RenderFramesObjects[renderFrameIndex];
        vkWaitForFences(getRenderSubsystemObject()->getDevice(), 1, &renderFrameObjects.renderFinishedFence, VK_TRUE, UINT64_MAX);
        if (renderFrameObjects.commandBuffer != nullptr)
        {
            renderFrameObjects.commandBuffer->returnToCommandPool();
            renderFrameObjects.commandBuffer = nullptr;
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

            VkSemaphore semaphore = swapchain->acquireNextImage();
            if (semaphore == nullptr)
            {
                return false;
            }

            m_SwapchainImageReadySemaphores.add(semaphore);
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

        RenderFrameObjects& renderFrameObjects = m_RenderFramesObjects[getRenderSubsystemObject()->getRenderFrameIndex()];
        renderFrameObjects.commandBuffer = commandBuffer;
        return true;
    }
    bool RenderPipeline_RenderAPIObject_Vulkan::submitRenderCommandBuffer()
    {
        const int8 renderFrameIndex = getRenderSubsystemObject()->getRenderFrameIndex();
        RenderFrameObjects& renderFrameObjects = m_RenderFramesObjects[renderFrameIndex];
        vkResetFences(getRenderSubsystemObject()->getDevice(), 1, &renderFrameObjects.renderFinishedFence);

        constexpr VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = m_SwapchainImageReadySemaphores.getSize();
        submitInfo.pWaitSemaphores = m_SwapchainImageReadySemaphores.getData();
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFrameObjects.renderFinishedSemaphore;
        if (!renderFrameObjects.commandBuffer->submit(submitInfo, renderFrameObjects.renderFinishedFence, false))
        {
            JUMA_LOG(error, JSTR("Failed to submit render command buffer"));
            renderFrameObjects.commandBuffer->returnToCommandPool();
            renderFrameObjects.commandBuffer = nullptr;
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

            swapchain->presentCurrentImage(renderFrameObjects.renderFinishedSemaphore);
        }
        return true;
    }
}

#endif
