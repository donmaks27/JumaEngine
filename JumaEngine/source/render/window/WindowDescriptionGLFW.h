// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_USE_WINDOW_LIB_GLFW)

#include "GLFW/glfw3.h"
#include "WindowDescriptionBase.h"

namespace JumaEngine
{
    class WindowDescriptionGLFW : public WindowDescriptionBase
    {
    public:
        virtual ~WindowDescriptionGLFW() override = default;

        GLFWwindow* windowPtr = nullptr;
    };
}

#endif
