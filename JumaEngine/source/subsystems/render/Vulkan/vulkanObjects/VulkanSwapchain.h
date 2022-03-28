// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "subsystems/render/RenderPresentMode.h"
#include "jutils/jarray.h"
#include "jutils/jdelegate_multicast.h"
#include "jutils/math/vector2.h"

namespace JumaEngine
{
    struct RenderOptions;
    class Window_Vulkan;
    class VulkanRenderPass;
    class VulkanSwapchain;

    struct VulkanSwapchainSettings
    {
        VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
        VkSurfaceFormatKHR surfaceFormat = { VK_FORMAT_R8G8B8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
        math::uvector2 size = { 0, 0 };
        RenderPresentMode presentMode = RenderPresentMode::VSYNC;
    };

    CREATE_JUTILS_MULTICAST_DELEGATE_OneParam(OnVulkanSwapchainEvent, VulkanSwapchain*, swapchain);

    class VulkanSwapchain : public VulkanContextObject
    {
        friend Window_Vulkan;

    public:
        VulkanSwapchain() = default;
        virtual ~VulkanSwapchain() override;

        OnVulkanSwapchainEvent onRenderPassChanged;


        bool init(Window_Vulkan* window);

        VkSampleCountFlagBits getSampleCount() const { return m_CurrentSettings.sampleCount; }
        VkFormat getFormat() const { return m_CurrentSettings.surfaceFormat.format; }
        math::uvector2 getSize() const { return m_CurrentSettings.size; }
        int32 getImageCount() const { return m_SwapchainImages.getSize(); }

        VkSwapchainKHR get() const { return m_Swapchain; }
        VulkanRenderPass* getRenderPass() const { return m_RenderPass; }
        VkImage getSwapchainImage(const int32 imageIndex) const { return m_SwapchainImages.isValidIndex(imageIndex) ? m_SwapchainImages[imageIndex] : nullptr; }
        int8 getSwapchainImageIndex() const { return m_CurrentSwapchainImageIndex; }

        VkSemaphore getRenderFrameAvailableSemaphore(const int32 renderFrameIndex) const { return m_RenderFrameAvailableSemaphores.isValidIndex(renderFrameIndex) ? m_RenderFrameAvailableSemaphores[renderFrameIndex] : nullptr; }
        
        void applySettings(bool forceRecreate = false);
        bool isNeedToRecreate() const { return m_NeedToRecreate; }

        bool startRender(RenderOptions* options);
        void finishRender(RenderOptions* options);

    protected:

        virtual void clearInternal() override;

    private:

        Window_Vulkan* m_Window = nullptr;

        VkSwapchainKHR m_Swapchain = nullptr;
        VulkanRenderPass* m_RenderPass = nullptr;
        jarray<VkImage> m_SwapchainImages;
        int8 m_CurrentSwapchainImageIndex = -1;

        jarray<VkSemaphore> m_RenderFrameAvailableSemaphores;

        VkSampleCountFlagBits m_MaxSampleCount = VK_SAMPLE_COUNT_1_BIT;
        VulkanSwapchainSettings m_CurrentSettings;
        VulkanSwapchainSettings m_SettingForApply;
        bool m_NeedToRecreate = false;


        VkSampleCountFlagBits getMaxSampleCount() const;
        static constexpr VkPresentModeKHR getVulkanPresentMode(RenderPresentMode presentMode);

        bool updateSwapchain();
        bool updateRenderPass();
        bool updateSyncObjects();

        void clearVulkanObjects();

        void onWindowSizeChanged();

        void markAsNeededToRecreate();
        bool applySettingsInternal(bool forceRecreate);
    };

    constexpr VkPresentModeKHR VulkanSwapchain::getVulkanPresentMode(const RenderPresentMode presentMode)
    {
        switch (presentMode)
        {
        case RenderPresentMode::VSYNC:         return VK_PRESENT_MODE_FIFO_KHR;
        case RenderPresentMode::TRIPLE_BUFFER: return VK_PRESENT_MODE_MAILBOX_KHR;
        default: ;
        }
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }
}

#endif
