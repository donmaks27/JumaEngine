// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "jutils/jarray.h"
#include "jutils/jdelegate_multicast.h"
#include "jutils/math/vector2.h"

namespace JumaEngine
{
    class VulkanRenderImage;
    struct RenderOptions;
    class VulkanFramebuffer;
    class Window;
    class Window_Vulkan;
    class VulkanRenderPass;
    class VulkanCommandBuffer;
    class VulkanSwapchainFramebuffer;
    class VulkanSwapchain;

    struct VulkanSwapchainSettings
    {
        VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
        uint32 imageCount = 0;
        VkSurfaceFormatKHR surfaceFormat = { VK_FORMAT_R8G8B8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
        math::uvector2 size = { 0, 0 };
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
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

        int8 getMaxRenderedFrameCount() const { return m_MaxRenderFrameCount; }

        VkSampleCountFlagBits getSampleCount() const { return m_CurrentSettings.sampleCount; }
        VkFormat getFormat() const { return m_CurrentSettings.surfaceFormat.format; }
        math::uvector2 getSize() const { return m_CurrentSettings.size; }
        uint32 getImageCount() const { return m_CurrentSettings.imageCount; }

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
        VulkanRenderImage* m_RenderImage = nullptr;

        int8 m_MaxRenderFrameCount = 3;
        int8 m_RenderFrameCount = 2;
        int8 m_RenderFrameIndex = 0;
        jarray<VkSemaphore> m_RenderFrameAvailableSemaphores;

        VkSampleCountFlagBits m_MaxSampleCount = VK_SAMPLE_COUNT_1_BIT;
        VulkanSwapchainSettings m_CurrentSettings;
        VulkanSwapchainSettings m_SettingForApply;
        bool m_NeedToRecreate = false;


        VkSampleCountFlagBits getMaxSampleCount() const;

        bool updateSwapchain();
        bool updateRenderPass();
        bool updateRenderImage();
        bool updateSyncObjects();

        void clearVulkanObjects();

        void onWindowSizeChanged();

        void markAsNeededToRecreate();
        bool applySettingsInternal(bool forceRecreate);
    };
}

#endif
