// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/WindowDescription.h"

#include <vulkan/vulkan_core.h>

namespace JumaEngine
{
    class WindowDescription_Vulkan : public WindowDescription
    {
    public:
        WindowDescription_Vulkan() = default;
        virtual ~WindowDescription_Vulkan() override = default;

        VkSurfaceKHR surface = nullptr;
    };
}

#endif