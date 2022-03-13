// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "WindowSubsystem_OpenGL.h"

#include "jutils/jmap.h"

namespace JumaEngine
{
    class Window_OpenGL_GLFW;

    class WindowSubsystem_OpenGL_GLFW : public WindowSubsystem_OpenGL
    {
        JUMAENGINE_CLASS(WindowSubsystem_OpenGL_GLFW, WindowSubsystem_OpenGL)

    public:
        WindowSubsystem_OpenGL_GLFW() = default;
        virtual ~WindowSubsystem_OpenGL_GLFW() override = default;

        virtual Window* createWindow(const jstring& title, const math::uvector2& size) override;
        virtual Window* findWindow(window_id_type windowID) const override;
        virtual void destroyWindow(window_id_type windowID) override;

    protected:

        virtual bool initSubsystemInternal() override;
        virtual void clearSubsystemInternal() override;

    private:

        jmap<window_id_type, Window_OpenGL_GLFW*> m_Windows;


        static void GLFW_ErrorCallback(int errorCode, const char* errorMessage);
    };
}

#endif
