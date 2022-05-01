// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/window/WindowSubsystem.h"

namespace JumaEngine
{
    struct WindowDescription_OpenGL : WindowDescription_RenderAPI
    {
    };

    class WindowSubsystem_RenderAPIObject_OpenGL : public WindowSubsystem_RenderAPIObject
    {
        using Super = WindowSubsystem_RenderAPIObject;

    public:
        WindowSubsystem_RenderAPIObject_OpenGL() = default;
        virtual ~WindowSubsystem_RenderAPIObject_OpenGL() override = default;

        void setWindowActive(window_id_type windowID);
        window_id_type getActiveWindow() const { return m_ActiveWindow; }

    protected:

        void initOpenGL() const;

        virtual void setWindowActiveInternal(window_id_type windowID) = 0;

    private:

        window_id_type m_ActiveWindow = INVALID_WINDOW_ID;
    };
}

#endif
