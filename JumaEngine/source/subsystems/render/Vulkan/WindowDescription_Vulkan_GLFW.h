// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "WindowDescription_Vulkan.h"

#include <GLFW/glfw3.h>

namespace JumaEngine
{
    class WindowDescription_Vulkan_GLFW : public WindowDescription_Vulkan
    {
    public:
        WindowDescription_Vulkan_GLFW() = default;
        virtual ~WindowDescription_Vulkan_GLFW() override = default;

        GLFWwindow* window = nullptr;
    };
}

#endif