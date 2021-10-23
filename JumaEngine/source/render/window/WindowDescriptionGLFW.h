// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "GLFW/glfw3.h"
#include "WindowDescriptionBaseOld.h"

namespace JumaEngine
{
    class WindowDescriptionGLFW : public WindowDescriptionBaseOld
    {
    public:
        WindowDescriptionGLFW() = default;
        virtual ~WindowDescriptionGLFW() override = default;

        GLFWwindow* windowPtr = nullptr;
    };
}

#endif
