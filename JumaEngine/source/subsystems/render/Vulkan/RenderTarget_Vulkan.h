// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderTarget.h"

namespace JumaEngine
{
    class RenderTarget_Vulkan : public RenderTarget
    {
        JUMAENGINE_CLASS(RenderTarget_Vulkan, RenderTarget)

    public:
        RenderTarget_Vulkan() = default;
        virtual ~RenderTarget_Vulkan() override = default;
    };
}

#endif
