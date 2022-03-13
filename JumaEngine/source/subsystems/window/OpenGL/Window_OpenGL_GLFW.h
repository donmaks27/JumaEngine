// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "Window_OpenGL.h"

struct GLFWwindow;

namespace JumaEngine
{
    class Window_OpenGL_GLFW : public Window_OpenGL
    {
        JUMAENGINE_CLASS(Window_OpenGL_GLFW, Window_OpenGL)

    public:
        Window_OpenGL_GLFW() = default;
        virtual ~Window_OpenGL_GLFW() override;

        virtual bool isValid() const override { return isValid_GLFW(); }

        bool initWindow(window_id_type windowID, const jstring& title, const math::uvector2& size);

        virtual bool shouldClose() const override;
        virtual void destroy() override;

        virtual void finishRender() override;

    protected:

        GLFWwindow* m_WindowGLFW = nullptr;

    private:

        bool isValid_GLFW() const { return m_WindowGLFW != nullptr; }

        static void GLFW_FramebufferResizeCallback(GLFWwindow* windowGLFW, int width, int height);

        void destroyWindow_GLFW();
    };
}

#endif
