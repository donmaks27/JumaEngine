// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem_OpenGL_GLFW.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "jutils/jlog.h"
#include "WindowDescription_OpenGL_GLFW.h"

namespace JumaEngine
{
    bool RenderSubsystem_OpenGL_GLFW::initSubsystemInternal()
    {
        const int initResult = glfwInit();
        if (initResult == GLFW_FALSE)
        {
            const char* errorStr = nullptr;
            glfwGetError(&errorStr);
            JUMA_LOG(error, errorStr);
            return false;
        }

        glfwSetErrorCallback(RenderSubsystem_OpenGL_GLFW::GLFW_ErrorCallback);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 0);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        createMainWindow();

        if (!Super::initSubsystemInternal())
        {
            terminateMainWindow();
            terminateGLFW();
            return false;
        }
        return true;
    }
    void RenderSubsystem_OpenGL_GLFW::GLFW_ErrorCallback(int code, const char* errorMessage)
    {
        JUMA_LOG(error, JSTR("Code: ") + TO_JSTR(code) + JSTR(". ") + errorMessage);
    }

    void RenderSubsystem_OpenGL_GLFW::clearSubsystemInternal()
    {
        terminateMainWindow();
        terminateGLFW();

        Super::clearSubsystemInternal();
    }
    void RenderSubsystem_OpenGL_GLFW::terminateGLFW()
    {
        glfwTerminate();
    }

    WindowDescription* RenderSubsystem_OpenGL_GLFW::createWindowInternal(const math::uvector2& size, const jstring& title)
    {
        GLFWwindow* window = glfwCreateWindow(size.x, size.y, *title, nullptr, nullptr);
        if (window == nullptr)
        {
            JUMA_LOG(warning, JSTR("Fail to create GLFW window"));
            return nullptr;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        WindowDescription_OpenGL_GLFW* windowDescription = new WindowDescription_OpenGL_GLFW();
        windowDescription->size = size;
        windowDescription->title = title;
        windowDescription->window = window;
        return windowDescription;
    }
    void RenderSubsystem_OpenGL_GLFW::terminateWindowInternal(WindowDescription* window)
    {
        WindowDescription_OpenGL_GLFW* window_GLFW = castWindow<WindowDescription_OpenGL_GLFW>(window);
        if ((window_GLFW != nullptr) && (window_GLFW->window != nullptr))
        {
            glfwDestroyWindow(window_GLFW->window);
            window_GLFW->window = nullptr;
        }
    }
    
    bool RenderSubsystem_OpenGL_GLFW::shouldCloseWindowInternal(WindowDescription* window) const
    {
        WindowDescription_OpenGL_GLFW* window_GLFW = castWindow<WindowDescription_OpenGL_GLFW>(window);
        if ((window_GLFW != nullptr) && (window_GLFW->window != nullptr))
        {
            return glfwWindowShouldClose(window_GLFW->window) != GLFW_FALSE;
        }
        return false;
    }
    void RenderSubsystem_OpenGL_GLFW::setWindowSizeInternal(WindowDescription* window, const math::uvector2& size)
    {
        WindowDescription_OpenGL_GLFW* window_GLFW = castWindow<WindowDescription_OpenGL_GLFW>(window);
        if ((window_GLFW != nullptr) && (window_GLFW->window != nullptr))
        {
            glfwSetWindowSize(window_GLFW->window, size.x, size.y);
        }
    }
    void RenderSubsystem_OpenGL_GLFW::setWindowTitleInternal(WindowDescription* window, const jstring& title)
    {
        WindowDescription_OpenGL_GLFW* window_GLFW = castWindow<WindowDescription_OpenGL_GLFW>(window);
        if ((window_GLFW != nullptr) && (window_GLFW->window != nullptr))
        {
            glfwSetWindowTitle(window_GLFW->window, *title);
        }
    }

    void RenderSubsystem_OpenGL_GLFW::render()
    {
        Super::render();

        WindowDescription_OpenGL_GLFW* window_GLFW = castWindow<WindowDescription_OpenGL_GLFW>(getMainWindow());
        if ((window_GLFW != nullptr) && (window_GLFW->window != nullptr))
        {
            glfwSwapBuffers(window_GLFW->window);
        }
        glfwPollEvents();
    }
}

#endif
