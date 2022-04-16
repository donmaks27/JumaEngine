// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

namespace JumaEngine
{
    class WindowSubsystem_RenderAPIObject_OpenGL;
    class RenderSubsystem_RenderAPIObject_OpenGL;

    class OpenGLContextObject
    {
        friend RenderSubsystem_RenderAPIObject_OpenGL;

    public:
        OpenGLContextObject() = default;
        virtual ~OpenGLContextObject() = default;

        RenderSubsystem_RenderAPIObject_OpenGL* getRenderSubsystemObject() const { return m_RenderSubsystem; }
        WindowSubsystem_RenderAPIObject_OpenGL* getWindowSubsystemObject() const { return m_WindowSubsystem; }

    private:

        RenderSubsystem_RenderAPIObject_OpenGL* m_RenderSubsystem = nullptr;
        WindowSubsystem_RenderAPIObject_OpenGL* m_WindowSubsystem = nullptr;
    };
}

#endif
