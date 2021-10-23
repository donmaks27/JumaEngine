// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "GLFW/glfw3.h"
#include "WindowDescriptionBaseOld.h"

namespace JumaEngine
{
    class WindowDescriptionGLFWOld : public WindowDescriptionBaseOld
    {
    public:
        WindowDescriptionGLFWOld() = default;
        virtual ~WindowDescriptionGLFWOld() override = default;

        GLFWwindow* windowPtr = nullptr;
    };
}

#endif
