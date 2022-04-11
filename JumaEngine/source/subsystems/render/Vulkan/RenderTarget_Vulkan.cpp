// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "RenderTarget_Vulkan.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "RenderSubsystem_Vulkan.h"
#include "vulkanObjects/VulkanImage.h"
#include "vulkanObjects/VulkanRenderImage.h"
#include "vulkanObjects/VulkanRenderPassDescription.h"

namespace JumaEngine
{
    RenderTarget_RenderAPIObject_Vulkan::~RenderTarget_RenderAPIObject_Vulkan()
    {
        clearData();
    }

    bool RenderTarget_RenderAPIObject_Vulkan::initInternal()
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
        renderPassDescription.shouldResolveMultisampling = m_Parent->shouldResolveMultisampling();
        renderPassDescription.renderToSwapchain = false;
        VulkanRenderPass* renderPass = getRenderSubsystemObject()->getRenderPass(renderPassDescription);
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

    void RenderTarget_RenderAPIObject_Vulkan::clearData()
    {
        if (m_RenderImage != nullptr)
        {
            delete m_RenderImage;
            m_RenderImage = nullptr;
        }
    }
}

#endif
