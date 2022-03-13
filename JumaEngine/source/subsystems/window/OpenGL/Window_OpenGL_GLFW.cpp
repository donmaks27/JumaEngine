// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Window_OpenGL_GLFW.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include <GLFW/glfw3.h>

namespace JumaEngine
{
    Window_OpenGL_GLFW::~Window_OpenGL_GLFW()
    {
        destroyWindow_GLFW();
    }

    bool Window_OpenGL_GLFW::initWindow(const window_id_type windowID, const jstring& title, const math::uvector2& size)
    {
        if (isValid_GLFW())
        {
            JUMA_LOG(warning, JSTR("Window already initialized!"));
            return false;
        }
        if (windowID == INVALID_WINDOW_ID)
        {
            JUMA_LOG(error, JSTR("Invalid windowID parameter"));
            return false;
        }

        glfwWindowHint(GLFW_SAMPLES, 0);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        GLFWwindow* window = glfwCreateWindow(size.x, size.y, *title, nullptr, nullptr);
        if (window == nullptr)
        {
            JUMA_LOG(error, JSTR("Failed to create GLFW window"));
            return false;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, Window_OpenGL_GLFW::GLFW_FramebufferResizeCallback);

        m_WindowID = windowID;
        m_Title = title;
        m_Size = size;
        m_WindowGLFW = window;
        return true;
    }

    void Window_OpenGL_GLFW::GLFW_FramebufferResizeCallback(GLFWwindow* windowGLFW, int width, int height)
    {
        Window_OpenGL_GLFW* window = static_cast<Window_OpenGL_GLFW*>(glfwGetWindowUserPointer(windowGLFW));
        if (window != nullptr)
        {
            window->onWindowResized({ math::max<uint32>(width, 0), math::max<uint32>(height, 0) });
        }
    }

    bool Window_OpenGL_GLFW::shouldClose() const
    {
        return isValid_GLFW() && (glfwWindowShouldClose(m_WindowGLFW) != GLFW_FALSE);
    }

    void Window_OpenGL_GLFW::destroy()
    {
        destroyWindow_GLFW();
    }
    void Window_OpenGL_GLFW::destroyWindow_GLFW()
    {
        if (m_WindowGLFW != nullptr)
        {
            glfwDestroyWindow(m_WindowGLFW);
            m_WindowGLFW = nullptr;
        }
    }

    void Window_OpenGL_GLFW::finishRender()
    {
        if (m_WindowGLFW != nullptr)
        {
            glfwSwapBuffers(m_WindowGLFW);
        }
        glfwPollEvents();
    }
}

#endif
