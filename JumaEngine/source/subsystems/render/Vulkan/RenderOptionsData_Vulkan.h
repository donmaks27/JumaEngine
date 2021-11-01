// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderOptions.h"

namespace JumaEngine
{
    struct RenderOptionsData_Vulkan : public RenderOptionsData
    {
        uint32 swapchainImageIndex = 0;
    };
}

#endif