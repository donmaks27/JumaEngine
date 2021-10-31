// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "utils/jshared_ptr.h"

namespace JumaEngine
{
    class Image_Vulkan;
    class WindowDescription_Vulkan;
    class WindowDescription;

    class VulkanSwapchain : public VulkanContextObject
    {
    public:
        VulkanSwapchain() = default;
        virtual ~VulkanSwapchain() override;

        bool init(const jshared_ptr<WindowDescription>& window);

    protected:

        virtual void clearInternal() override;

    private:

        jshared_ptr<WindowDescription> m_Window;

        VkSwapchainKHR m_Swapchain = nullptr;
        jshared_ptr<Image_Vulkan> m_RenderImage_Color = nullptr;
        jshared_ptr<Image_Vulkan> m_RenderImage_Depth = nullptr;

        VkSampleCountFlagBits m_MaxSampleCount = VK_SAMPLE_COUNT_1_BIT;
        VkSampleCountFlagBits m_CurrentSampleCount = VK_SAMPLE_COUNT_1_BIT;
        VkFormat m_Format = VK_FORMAT_R8G8B8_SRGB;
        VkExtent2D m_Size = { 0, 0 };


        VkSampleCountFlagBits getMaxSampleCount() const;
        WindowDescription_Vulkan* getWindow() const;

        bool createSwapchain();

        void clearSwapchain();
    };
}

#endif
