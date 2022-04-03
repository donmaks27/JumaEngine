// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderTargetRenderAPIObject_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "RenderSubsystem_Vulkan.h"
#include "vulkanObjects/VulkanImage.h"
#include "vulkanObjects/VulkanRenderImage.h"
#include "vulkanObjects/VulkanRenderPassDescription.h"

namespace JumaEngine
{
    bool RenderTargetRenderAPIObject_Vulkan::initInternal()
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
        renderPassDescription.renderToSwapchain = false;
        renderPassDescription.sampleCount = VK_SAMPLE_COUNT_1_BIT;
        VulkanRenderPass* renderPass = getRenderSubsystemObject()->createRenderPass(renderPassDescription);
        if (renderPass == nullptr)
        {
            JUMA_LOG(error, JSTR("Failed to get render pass"));
            return false;
        }

        VulkanRenderImage* renderImage = getRenderSubsystemObject()->createVulkanObject<VulkanRenderImage>();
        if (!renderImage->init(renderPass, m_Parent->getSize()) || !renderImage->update())
        {
            JUMA_LOG(error, JSTR("Failed to initialize render image"));
            delete renderImage;
            return false;
        }

        m_RenderImage = renderImage;
        return true;
    }
}

#endif
