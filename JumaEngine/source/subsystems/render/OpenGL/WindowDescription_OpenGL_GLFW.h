// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "WindowDescription_OpenGL.h"

#include <GLFW/glfw3.h>

namespace JumaEngine
{
    class WindowDescription_OpenGL_GLFW : public WindowDescription_OpenGL
    {
    public:
        WindowDescription_OpenGL_GLFW() = default;
        virtual ~WindowDescription_OpenGL_GLFW() override = default;

        GLFWwindow* window = nullptr;
    };
}

#endif