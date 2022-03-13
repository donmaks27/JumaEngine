// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/window/Window.h"

namespace JumaEngine
{
    class Window_OpenGL : public Window
    {
        JUMAENGINE_ABSTRACT_CLASS(Window_OpenGL, Window)

    public:
        Window_OpenGL() = default;
        virtual ~Window_OpenGL() override = default;
    };
}

#endif
