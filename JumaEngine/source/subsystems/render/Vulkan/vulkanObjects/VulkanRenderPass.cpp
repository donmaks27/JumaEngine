﻿// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "VulkanRenderPass.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"

namespace JumaEngine
{
    VulkanRenderPass::~VulkanRenderPass()
    {
        clearRenderPass();
    }

    bool VulkanRenderPass::init(const VulkanRenderPassDescription& description, const render_pass_id_type typeID)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Render pass already initialized"));
            return false;
        }

        const bool multisampleEnabled = description.sampleCount > VK_SAMPLE_COUNT_1_BIT;

        VkAttachmentDescription attachments[3];
        const int32 attachmentCount = multisampleEnabled ? 3 : 2;

        VkAttachmentDescription& colorAttachment = attachments[0];
        colorAttachment.flags = 0;
        colorAttachment.format = description.colorFormat;
        colorAttachment.samples = description.sampleCount;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        if (multisampleEnabled || !description.renderToSwapchain)
        {
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
        else
        {
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }

        VkAttachmentDescription& depthAttachment = attachments[1];
        depthAttachment.flags = 0;
        depthAttachment.format = description.depthFormat;
        depthAttachment.samples = description.sampleCount;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        if (multisampleEnabled)
        {
            VkAttachmentDescription& colorResolveAttachment = attachments[2];
            colorResolveAttachment.flags = 0;
            colorResolveAttachment.format = description.colorFormat;
            colorResolveAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorResolveAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorResolveAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorResolveAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorResolveAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorResolveAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorResolveAttachment.finalLayout = !description.renderToSwapchain ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }
        
        constexpr VkAttachmentReference attachmentRefs[3] = {
            { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
            { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
            { 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
        };

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &attachmentRefs[0];
        subpass.pResolveAttachments = multisampleEnabled ? &attachmentRefs[2] : nullptr;
        subpass.pDepthStencilAttachment = &attachmentRefs[1];

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = attachmentCount;
        renderPassInfo.pAttachments = attachments;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        const VkResult result = vkCreateRenderPass(getRenderSubsystem()->getDevice(), &renderPassInfo, nullptr, &m_RenderPass);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create render pass"), result);
            return false;
        }

        m_Description = description;
        m_TypeID = typeID;
        markAsInitialized();
        return true;
    }

    void VulkanRenderPass::clearRenderPass()
    {
        if (m_RenderPass != nullptr)
        {
            vkDestroyRenderPass(getRenderSubsystem()->getDevice(), m_RenderPass, nullptr);
            m_RenderPass = nullptr;

            m_TypeID = INVALID_RENDER_PASS_TYPE_ID;
        }
    }

    VulkanFramebufferImagesDescription VulkanRenderPass::getImagesDescription() const
    {
        if (!isValid())
        {
            return VulkanFramebufferImagesDescription();
        }

        VulkanFramebufferImagesDescription result;

        if (m_Description.sampleCount != VK_SAMPLE_COUNT_1_BIT)
        {
            result.images.resize(3);
            result.resultImageIndex = 2;

            VulkanFramebufferImageDescription& resolveImage = result.images[2];
            resolveImage.sampleCount = VK_SAMPLE_COUNT_1_BIT;
            resolveImage.format = m_Description.colorFormat;
            resolveImage.depthImage = false;
        }
        else
        {
            result.images.resize(2);
            result.resultImageIndex = 0;
        }

        VulkanFramebufferImageDescription& colorImage = result.images[0];
        colorImage.sampleCount = m_Description.sampleCount;
        colorImage.format = m_Description.colorFormat;
        colorImage.depthImage = false;

        VulkanFramebufferImageDescription& depthImage = result.images[1];
        depthImage.sampleCount = m_Description.sampleCount;
        depthImage.format = m_Description.depthFormat;
        depthImage.depthImage = true;

        return result;
    }
}

#endif
