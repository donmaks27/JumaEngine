// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderTarget.h"
#include "VulkanContextObject.h"

namespace JumaEngine
{
    class VulkanRenderImage;

    class RenderTarget_RenderAPIObject_Vulkan : public RenderTarget_RenderAPIObject, public VulkanContextObjectBase
    {
    public:
        RenderTarget_RenderAPIObject_Vulkan() = default;
        virtual ~RenderTarget_RenderAPIObject_Vulkan() override;

        VulkanRenderImage* getRenderImage() const { return m_RenderImage; }

    protected:

        virtual bool initInternal() override;

    private:

        VulkanRenderImage* m_RenderImage = nullptr;


        void clearData();
    };
}

#endif
