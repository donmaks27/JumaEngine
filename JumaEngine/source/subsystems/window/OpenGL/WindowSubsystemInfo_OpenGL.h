// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/window/WindowSubsystemInfo.h"

#include "WindowSubsystem_OpenGL_GLFW.h"

namespace JumaEngine
{
#if defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)
    template<>
    struct WindowSubsystemInfo<RenderAPI::OpenGL> : std::true_type
    {
        static WindowSubsystem_RenderAPIObject* createRenderAPIObject() { return new WindowSubsystem_RenderAPIObject_OpenGL_GLFW(); }
    };
#endif
}

#endif
