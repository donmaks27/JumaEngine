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
    class VulkanSwapchain;

    class VulkanSwapchainFramebuffer : public VulkanContextObject
    {
    public:
        VulkanSwapchainFramebuffer() = default;
        virtual ~VulkanSwapchainFramebuffer() override;

        bool init(VulkanSwapchain* swapchain, VkImage swapchainImage);

        const jshared_ptr<Image_Vulkan>& getImage() const { return m_Image; }
        VkFramebuffer get() const { return m_Framebuffer; }

    protected:

        virtual void clearInternal() override { clearFramebuffer(); }

    private:

        VulkanSwapchain* m_ParentSwapchain = nullptr;
        jshared_ptr<Image_Vulkan> m_Image = nullptr;
        VkFramebuffer m_Framebuffer = nullptr;


        void clearFramebuffer();
    };
}

#endif
