// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "WindowDescription.h"

#include <GLFW/glfw3.h>

namespace JumaEngine
{
    class WindowDescription_GLFW : public WindowDescription
    {
    public:
        WindowDescription_GLFW() = default;
        virtual ~WindowDescription_GLFW() override = default;

        GLFWwindow* window = nullptr;
    };
}

#endif
