// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "WindowGLFW.h"
#include "GLFW/glfw3.h"
#include "utils/log.h"

namespace JumaEngine
{
    WindowGLFW::WindowGLFW()
    {
    }
    WindowGLFW::~WindowGLFW()
    {
        glfwTerminate();

        m_WindowObject = nullptr;
    }

    bool WindowGLFW::init()
    {
        if (isInitialized())
        {
            JUMA_LOG(warning, TEXT("WindowGLFW already initialized!"));
            return false;
        }

        if (!isInitialized())
        {
            const int initResult = glfwInit();
            if (initResult == GLFW_FALSE)
            {
                const char* errorStr = nullptr;
                glfwGetError(&errorStr);
                JUMA_LOG(error, errorStr);
                return false;
            }

            glfwSetErrorCallback(WindowGLFW::ErrorCallback);

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_SAMPLES, 0);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            m_WindowObject = CreateWindow();
            if (m_WindowObject == nullptr)
            {
                return false;
            }

            glfwMakeContextCurrent(m_WindowObject);
            glfwSwapInterval(0);

            return true;
        }
        return false;
    }
    void WindowGLFW::ErrorCallback(int code, const char* errorMessage)
    {
        JUMA_LOG(error, errorMessage);
    }

    GLFWwindow* WindowGLFW::CreateWindow() const
    {
        return glfwCreateWindow(800, 600, "Title", nullptr, nullptr);
    }

    bool WindowGLFW::shouldCloseWindow() const
    {
        return !isInitialized() || glfwWindowShouldClose(m_WindowObject);
    }
    
    void WindowGLFW::onFrameRenderFinish()
    {
        if (isInitialized())
        {
            glfwSwapBuffers(m_WindowObject);
            glfwPollEvents();
        }
    }
}
