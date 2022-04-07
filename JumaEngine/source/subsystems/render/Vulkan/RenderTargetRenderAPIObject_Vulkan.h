// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderTarget.h"
#include "VulkanContextObject.h"

namespace JumaEngine
{
    class VulkanRenderImage;

    class RenderTargetRenderAPIObject_Vulkan : public RenderTargetRenderAPIObject, public VulkanContextObjectBase
    {
    public:
        RenderTargetRenderAPIObject_Vulkan() = default;
        virtual ~RenderTargetRenderAPIObject_Vulkan() override;

        VulkanRenderImage* getRenderImage() const { return m_RenderImage; }

    protected:

        virtual bool initInternal() override;

    private:

        VulkanRenderImage* m_RenderImage = nullptr;


        void clearData();
    };
}

#endif
