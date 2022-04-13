// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderPipeline.h"
#include "VulkanContextObject.h"

#include <vulkan/vulkan_core.h>

namespace JumaEngine
{
    class VulkanCommandBuffer;

    class RenderPipeline_RenderAPIObject_Vulkan : public RenderPipeline_RenderAPIObject, public VulkanContextObjectBase
    {
    public:
        RenderPipeline_RenderAPIObject_Vulkan() = default;
        virtual ~RenderPipeline_RenderAPIObject_Vulkan() override;

    protected:

        virtual bool initInternal() override;
        virtual bool onRenderPipelineUpdated() override;

        virtual bool renderPipeline() override;

    private:

        jarray<VkSemaphore> m_SwapchainImageReadySemaphores;
        VkFence m_RenderFinishedFence = nullptr;
        VkSemaphore m_RenderFinishedSemaphore = nullptr;
        VulkanCommandBuffer* m_RenderCommandBuffer = nullptr;


        void clearData();

        bool update() { return updateSyncObjects(); }
        bool updateSyncObjects();

        bool waitForRenderFinish();
        bool acquireNextSwapchainImages();
        bool recordRenderCommandBuffer();
        bool submitRenderCommandBuffer();
    };
}

#endif
