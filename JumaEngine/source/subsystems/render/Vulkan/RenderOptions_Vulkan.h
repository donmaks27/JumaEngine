// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderOptions.h"

namespace JumaEngine
{
    class VulkanRenderImage;

    struct RenderOptions_Vulkan : public RenderOptions
    {
        VulkanRenderImage* renderImage = nullptr;
    };
}

#endif
