// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/Vulkan/VulkanContextObject.h"

#include <vulkan/vulkan_core.h>
#include <glm/vec2.hpp>

#include "utils/jshared_ptr.h"
#include "utils/jarray.h"
#include "utils/jdelegate_multicast.h"
#include "subsystems/render/RenderOptions.h"

namespace JumaEngine
{
    class VulkanCommandBuffer;
    class VulkanSwapchainFramebuffer;
    class Image_Vulkan;
    class WindowDescription_Vulkan;
    class WindowDescription;
    class VulkanSwapchain;

    struct VulkanSwapchainSettings
    {
        VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
        uint32 imageCount = 0;
        VkSurfaceFormatKHR surfaceFormat = { VK_FORMAT_R8G8B8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
        glm::uvec2 size = { 0, 0 };
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    };

    CREATE_JUTILS_MULTICAST_DELEGATE_OneParam(OnVulkanSwapchainEvent, VulkanSwapchain*, swapchain);

    class VulkanSwapchain : public VulkanContextObject
    {
    public:
        VulkanSwapchain() = default;
        virtual ~VulkanSwapchain() override;

        OnVulkanSwapchainEvent onRenderPassChanged;


        bool init(WindowDescription* window);

        VkSampleCountFlagBits getSampleCount() const { return m_CurrentSettings.sampleCount; }
        VkFormat getFormat() const { return m_CurrentSettings.surfaceFormat.format; }
        glm::uvec2 getSize() const { return m_CurrentSettings.size; }
        uint32 getImageCount() const { return m_CurrentSettings.imageCount; }

        VkSwapchainKHR get() const { return m_Swapchain; }
        const jshared_ptr<Image_Vulkan>& getRenderColorImage() const { return m_RenderImage_Color; }
        const jshared_ptr<Image_Vulkan>& getRenderDepthImage() const { return m_RenderImage_Depth; }
        VkRenderPass getRenderPass() const { return m_RenderPass; }

        void applySettings(bool forceRecreate = false);
        bool isNeedToRecreate() const { return m_NeedToRecreate; }

        bool startRender(const RenderOptions& options);
        void finishRender(const RenderOptions& options);

    protected:

        virtual void clearInternal() override;

    private:

        WindowDescription* m_Window = nullptr;

        VkSwapchainKHR m_Swapchain = nullptr;
        jshared_ptr<Image_Vulkan> m_RenderImage_Color = nullptr;
        jshared_ptr<Image_Vulkan> m_RenderImage_Depth = nullptr;
        VkRenderPass m_RenderPass = nullptr;
        jarray<jshared_ptr<VulkanSwapchainFramebuffer>> m_Framebuffers;

        uint32 m_MaxFramesInFlight = 2;
        uint32 m_CurrentInFlightFrame = 0;
        jarray<VkSemaphore> m_Semaphores_ImageAvailable;
        jarray<VkSemaphore> m_Semaphores_RenderFinished;
        jarray<VkFence> m_Fences_RenderFinished;

        VkSampleCountFlagBits m_MaxSampleCount = VK_SAMPLE_COUNT_1_BIT;
        VulkanSwapchainSettings m_CurrentSettings;
        VulkanSwapchainSettings m_SettingForApply;
        bool m_NeedToRecreate = false;


        VkSampleCountFlagBits getMaxSampleCount() const;
        WindowDescription_Vulkan* getWindow() const;

        bool createSwapchain();
        bool createRenderImages();
        bool createRenderPass();
        bool createFramebuffers();
        bool createSyncObjects();

        void clearSwapchain();

        void onWindowSizeChanged(WindowDescription* window);

        void markAsNeededToRecreate();
        bool applySettingsInternal(bool forceRecreate);

        jshared_ptr<VulkanCommandBuffer> createRenderCommandBuffer(uint32 swapchainImageIndex);
    };
}

#endif
