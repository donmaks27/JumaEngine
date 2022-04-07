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
    class VulkanRenderImage;

    class RenderPipelineRenderAPIObject_Vulkan : public RenderPipelineRenderAPIObject, public VulkanContextObjectBase
    {
    public:
        RenderPipelineRenderAPIObject_Vulkan() = default;
        virtual ~RenderPipelineRenderAPIObject_Vulkan() override;

    protected:

        virtual bool initInternal() override;
        virtual bool onRenderPipelineUpdated() override;

        virtual bool renderPipeline() override;

    private:

        struct RenderFrameObjects
        {
            VkFence renderFinishedFence = nullptr;
            VkSemaphore renderFinishedSemaphore = nullptr;

            VulkanCommandBuffer* commandBuffer = nullptr;
        };

        jmap<jstringID, VulkanRenderImage*> m_RenderImages;

        jarray<RenderFrameObjects> m_RenderFramesObjects;
        jarray<VkSemaphore> m_SwapchainImageReadySemaphores;


        void clearData();

        bool update() { return updateRenderImages() && updateSyncObjects(); }
        bool updateRenderImages();
        bool updateSyncObjects();

        bool waitForRenderFinish();
        bool acquireNextSwapchainImages();
        bool recordRenderCommandBuffer();
        bool submitRenderCommandBuffer();
    };
}

#endif
