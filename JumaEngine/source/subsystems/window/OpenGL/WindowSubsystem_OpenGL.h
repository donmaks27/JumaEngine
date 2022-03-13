// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/window/WindowSubsystem.h"

namespace JumaEngine
{
    class WindowSubsystem_OpenGL : public WindowSubsystem
    {
        JUMAENGINE_ABSTRACT_CLASS(WindowSubsystem_OpenGL, WindowSubsystem)

    public:
        WindowSubsystem_OpenGL() = default;
        virtual ~WindowSubsystem_OpenGL() override = default;
    };
}

#endif
