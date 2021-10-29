// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/WindowDescription.h"

namespace JumaEngine
{
    class WindowDescription_OpenGL : public WindowDescription
    {
    public:
        WindowDescription_OpenGL() = default;
        virtual ~WindowDescription_OpenGL() override = default;
    };
}

#endif
