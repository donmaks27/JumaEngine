// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

#include "jutils/jshared_ptr.h"
#include "jutils/jarray.h"
#include "jutils/jdelegate_multicast.h"
#include "jutils/math/vector2.h"
#include "subsystems/render/RenderOptions.h"

namespace JumaEngine
{
    class VulkanFramebuffer;
    class Window;
    class Window_Vulkan;
    class VulkanRenderPass;
    class VulkanCommandBuffer;
    class VulkanSwapchainFramebuffer;
    class Image_Vulkan;
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

        VkSampleCountFlagBits getSampleCount() const { return m_CurrentSettings.sampleCount; }
        VkFormat getFormat() const { return m_CurrentSettings.surfaceFormat.format; }
        math::uvector2 getSize() const { return m_CurrentSettings.size; }
        uint32 getImageCount() const { return m_CurrentSettings.imageCount; }

        VkSwapchainKHR get() const { return m_Swapchain; }
        VulkanRenderPass* getRenderPass() const { return m_RenderPass; }

        void applySettings(bool forceRecreate = false);
        bool isNeedToRecreate() const { return m_NeedToRecreate; }

        bool startRender(const RenderOptions& options);
        void finishRender(const RenderOptions& options);

    protected:

        virtual void clearInternal() override;

    private:

        Window_Vulkan* m_Window = nullptr;

        VkSwapchainKHR m_Swapchain = nullptr;
        VulkanRenderPass* m_RenderPass = nullptr;
        jarray<jshared_ptr<VulkanFramebuffer>> m_Framebuffers;

        uint32 m_MaxFramesInFlight = 2;
        uint32 m_CurrentInFlightFrame = 0;
        jarray<int32> m_InFlightFrameIndices;
        jarray<VkSemaphore> m_Semaphores_ImageAvailable;
        jarray<VkSemaphore> m_Semaphores_RenderFinished;
        jarray<VkFence> m_Fences_RenderFinished;

        VkSampleCountFlagBits m_MaxSampleCount = VK_SAMPLE_COUNT_1_BIT;
        VulkanSwapchainSettings m_CurrentSettings;
        VulkanSwapchainSettings m_SettingForApply;
        bool m_NeedToRecreate = false;


        VkSampleCountFlagBits getMaxSampleCount() const;

        bool createSwapchain();
        bool createRenderPass();
        bool createFramebuffers();
        bool createSyncObjects();

        void clearVulkanObjects();

        void onWindowSizeChanged();

        void markAsNeededToRecreate();
        bool applySettingsInternal(bool forceRecreate);

        jshared_ptr<VulkanCommandBuffer> createRenderCommandBuffer(uint32 swapchainImageIndex);
    };
}

#endif
