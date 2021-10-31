// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>
#include <glm/vec2.hpp>

#include "utils/jshared_ptr.h"
#include "utils/jarray.h"

namespace JumaEngine
{
    class VulkanSwapchainFramebuffer;
    class Image_Vulkan;
    class WindowDescription_Vulkan;
    class WindowDescription;

    class VulkanSwapchain : public VulkanContextObject
    {
    public:
        VulkanSwapchain() = default;
        virtual ~VulkanSwapchain() override;

        bool init(const jshared_ptr<WindowDescription>& window);

        VkFormat getFormat() const { return m_Format; }
        glm::uvec2 getSize() const { return { m_Size.width, m_Size.height }; }

        VkSwapchainKHR get() const { return m_Swapchain; }
        const jshared_ptr<Image_Vulkan>& getRenderColorImage() const { return m_RenderImage_Color; }
        const jshared_ptr<Image_Vulkan>& getRenderDepthImage() const { return m_RenderImage_Depth; }
        VkRenderPass getRenderPass() const { return m_RenderPass; }

    protected:

        virtual void clearInternal() override;

    private:

        jshared_ptr<WindowDescription> m_Window;

        VkSwapchainKHR m_Swapchain = nullptr;
        jshared_ptr<Image_Vulkan> m_RenderImage_Color = nullptr;
        jshared_ptr<Image_Vulkan> m_RenderImage_Depth = nullptr;
        VkRenderPass m_RenderPass = nullptr;
        jarray<jshared_ptr<VulkanSwapchainFramebuffer>> m_Framebuffers;

        VkSampleCountFlagBits m_MaxSampleCount = VK_SAMPLE_COUNT_1_BIT;
        VkSampleCountFlagBits m_CurrentSampleCount = VK_SAMPLE_COUNT_1_BIT;
        VkFormat m_Format = VK_FORMAT_R8G8B8_SRGB;
        VkExtent2D m_Size = { 0, 0 };


        VkSampleCountFlagBits getMaxSampleCount() const;
        WindowDescription_Vulkan* getWindow() const;

        bool createSwapchain();
        bool createRenderPass();
        bool createFramebuffers();

        void clearSwapchain();
    };
}

#endif
