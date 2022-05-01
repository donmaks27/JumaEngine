﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "subsystems/render/RenderPresentMode.h"
#include "jutils/jarray.h"
#include "jutils/math/vector2.h"

namespace JumaEngine
{
    struct RenderOptions;
    class WindowSubsystem_RenderAPIObject_Vulkan;
    class VulkanSwapchain;

    constexpr VkPresentModeKHR GetVulkanPresentModeByRenderPresentMode(RenderPresentMode presentMode);

    struct VulkanSwapchainSettings
    {
        math::uvector2 size = { 0, 0 };
        RenderPresentMode presentMode = RenderPresentMode::VSync;
    };

    class VulkanSwapchain : public VulkanContextObject
    {
        friend WindowSubsystem_RenderAPIObject_Vulkan;

    public:
        VulkanSwapchain() = default;
        virtual ~VulkanSwapchain() override;

        bool init(VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, const math::uvector2& windowSize);

        VkFormat getFormat() const { return m_SwapchainImagesFormat.format; }
        const math::uvector2& getSize() const { return m_CurrentSettings.size; }
        int32 getImageCount() const { return m_SwapchainImages.getSize(); }

        VkSwapchainKHR get() const { return m_Swapchain; }
        VkImage getSwapchainImage(const int32 imageIndex) const { return m_SwapchainImages.isValidIndex(imageIndex) ? m_SwapchainImages[imageIndex] : nullptr; }
        int8 getSwapchainImageIndex() const { return m_CurrentSwapchainImageIndex; }

        VkSemaphore getRenderAvailableSemaphore() const { return m_RenderAvailableSemaphore; }
        
        void markAsNeededToRecreate();
        void applySettings(bool forceRecreate = false);
        bool isNeedToRecreate() const { return m_NeedToRecreate; }

        bool acquireNextImage();

    protected:

        virtual void clearInternal() override;

    private:

        VkSurfaceKHR m_WindowSurface = nullptr;

        VkSwapchainKHR m_Swapchain = nullptr;
        VkSurfaceFormatKHR m_SwapchainImagesFormat = { VK_FORMAT_R8G8B8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        jarray<VkImage> m_SwapchainImages;
        int8 m_CurrentSwapchainImageIndex = -1;

        VkSemaphore m_RenderAvailableSemaphore = nullptr;

        VulkanSwapchainSettings m_CurrentSettings;
        VulkanSwapchainSettings m_SettingForApply;
        bool m_NeedToRecreate = false;


        bool updateSwapchain();
        bool updateSyncObjects();

        void clearVulkanObjects();

        void onWindowSizeChanged(const math::uvector2& newSize);

        bool applySettingsInternal(bool forceRecreate);
    };

    constexpr VkPresentModeKHR GetVulkanPresentModeByRenderPresentMode(RenderPresentMode presentMode)
    {
        switch (presentMode)
        {
        case RenderPresentMode::VSync:         return VK_PRESENT_MODE_FIFO_KHR;
        case RenderPresentMode::TripleBuffer: return VK_PRESENT_MODE_MAILBOX_KHR;
        default: ;
        }
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }
}

#endif
