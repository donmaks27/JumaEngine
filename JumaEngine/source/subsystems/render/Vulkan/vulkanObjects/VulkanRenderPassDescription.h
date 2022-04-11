// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include <vulkan/vulkan_core.h>

#include "jutils/juid.h"

namespace JumaEngine
{
    using render_pass_id_type = uint32;
    constexpr render_pass_id_type INVALID_RENDER_PASS_TYPE_ID = juid<render_pass_id_type>::invalidUID;

    struct VulkanRenderPassDescription
    {
        VkFormat colorFormat = VK_FORMAT_UNDEFINED;
        VkFormat depthFormat = VK_FORMAT_UNDEFINED;
        VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;

        bool shouldUseDepth = true;
        bool shouldResolveMultisampling = false;
        bool renderToSwapchain = true;

        bool isResolveEnabled() const { return shouldResolveMultisampling && (sampleCount != VK_SAMPLE_COUNT_1_BIT); }

        struct CompatiblePredicate
        {
            constexpr bool operator()(const VulkanRenderPassDescription& description1, const VulkanRenderPassDescription& description2) const
            {
                if (description1.colorFormat != description2.colorFormat)
                {
                    return description1.colorFormat < description2.colorFormat;
                }
                if (description1.shouldUseDepth != description2.shouldUseDepth)
                {
                    return description2.shouldUseDepth;
                }
                if (description1.shouldUseDepth)
                {
                    if (description1.depthFormat != description2.depthFormat)
                    {
                        return description1.depthFormat < description2.depthFormat;
                    }
                }
                if (description1.sampleCount != description2.sampleCount)
                {
                    return description1.sampleCount < description2.sampleCount;
                }
                return !description1.isResolveEnabled() && description2.isResolveEnabled();
            }
        };
        struct EqualPredicate : CompatiblePredicate
        {
            constexpr bool operator()(const VulkanRenderPassDescription& description1, const VulkanRenderPassDescription& description2) const
            {
                if (CompatiblePredicate::operator()(description1, description2))
                {
                    return true;
                }
                return !description1.renderToSwapchain && description2.renderToSwapchain;
            }
        };
    };
}

#endif
