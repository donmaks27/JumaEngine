// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/window/WindowSubsystem.h"

#include "jutils/jarray.h"

namespace JumaEngine
{
    class WindowSubsystem_Vulkan : public WindowSubsystem
    {
        JUMAENGINE_ABSTRACT_CLASS(WindowSubsystem_Vulkan, WindowSubsystem)

    public:
        WindowSubsystem_Vulkan() = default;
        virtual ~WindowSubsystem_Vulkan() override = default;

        virtual jarray<const char*> getVulkanInstanceExtensions() const = 0;
    };
}

#endif
