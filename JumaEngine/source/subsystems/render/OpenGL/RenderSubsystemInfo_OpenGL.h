// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#include "subsystems/render/RenderSubsystemInfo.h"

#include "RenderSubsystem_OpenGL.h"

namespace JumaEngine
{
    template<>
    struct RenderSubsystemInfo<RenderAPI::OpenGL> : std::true_type
    {
        static RenderSubsystem_RenderAPIObject* createRenderAPIObject() { return new RenderSubsystem_RenderAPIObject_OpenGL(); }
    };
}

#endif
