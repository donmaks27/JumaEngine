// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "WindowSubsystem_OpenGL.h"

#include "jutils/jmap.h"

struct GLFWwindow;

namespace JumaEngine
{
    class WindowSubsystem_RenderAPIObject_OpenGL_GLFW;

    struct WindowDescription_OpenGL_GLFW : WindowDescription_OpenGL
    {
        GLFWwindow* windowGLFW = nullptr;
        WindowSubsystem_RenderAPIObject_OpenGL_GLFW* windowSubsystemObject = nullptr;
    };

    class WindowSubsystem_RenderAPIObject_OpenGL_GLFW : public WindowSubsystem_RenderAPIObject_OpenGL
    {
        using Super = WindowSubsystem_RenderAPIObject_OpenGL;

    public:
        WindowSubsystem_RenderAPIObject_OpenGL_GLFW() = default;
        virtual ~WindowSubsystem_RenderAPIObject_OpenGL_GLFW() override;

        virtual const WindowDescription_RenderAPI* findWindow(const window_id_type windowID) const override { return m_Windows.find(windowID); }

        virtual bool shouldCloseWindow(window_id_type windowID) const override;

    protected:
        
        virtual bool initInternal() override;

        virtual WindowDescription_RenderAPI* findWindow(const window_id_type windowID) override { return m_Windows.find(windowID); }

        virtual bool createWindow(window_id_type windowID) override;
        virtual void destroyWindow(window_id_type windowID) override;

        virtual void initWindowThread(WindowDescription_OpenGL* windowDescription) override;
        virtual void finishWindowThread(WindowDescription_OpenGL* windowDescription) override;

        virtual void onFinishWindowRender(window_id_type windowID) override;
        virtual void onFinishRender() override;

    private:

        jmap<window_id_type, WindowDescription_OpenGL_GLFW> m_Windows;


        static void GLFW_ErrorCallback(int errorCode, const char* errorMessage);
        static void GLFW_FramebufferResizeCallback(GLFWwindow* windowGLFW, int width, int height);

        void clearData_GLFW();
        void destroyWindow_GLFW(window_id_type windowID, WindowDescription_OpenGL_GLFW& description);
    };
}

#endif
