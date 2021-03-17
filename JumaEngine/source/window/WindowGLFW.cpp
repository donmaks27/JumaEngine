// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "WindowGLFW.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "utils/log.h"

namespace JumaEngine
{
    WindowGLFW::WindowGLFW()
    {
    }
    WindowGLFW::~WindowGLFW()
    {
        terminateInternal();
    }

    bool WindowGLFW::createWindow()
    {
        if (isWindowCreated())
        {
            JUMA_LOG(warning, JTEXT("WindowGLFW already initialized!"));
            return false;
        }

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
    void WindowGLFW::ErrorCallback(int code, const char* errorMessage)
    {
        JUMA_LOG(error, errorMessage);
    }

    GLFWwindow* WindowGLFW::CreateWindow() const
    {
        return glfwCreateWindow(800, 600, JTEXT("JUMAEngine"), nullptr, nullptr);
    }

    void WindowGLFW::setWindowTitle(const char* title)
    {
        if (isWindowCreated())
        {
            glfwSetWindowTitle(m_WindowObject, title);
        }
    }

    void WindowGLFW::onEngineLoopStart()
    {
        m_LastTimeStamp = glfwGetTime();
        m_DeltaTime = 0.0;
    }

    bool WindowGLFW::shouldCloseWindow() const
    {
        return !isWindowCreated() || glfwWindowShouldClose(m_WindowObject);
    }
    
    void WindowGLFW::onFrameRenderFinish()
    {
        if (isWindowCreated())
        {
            glfwSwapBuffers(m_WindowObject);
            glfwPollEvents();

            updateDeltaTime();
        }
    }
    void WindowGLFW::updateDeltaTime()
    {
        const double timeStamp = glfwGetTime();
        m_DeltaTime = timeStamp - m_LastTimeStamp;
        m_LastTimeStamp = timeStamp;
    }

    void WindowGLFW::termiante()
    {
        terminateInternal();
    }
    void WindowGLFW::terminateInternal()
    {
        if (isWindowCreated())
        {
            glfwTerminate();

            m_WindowObject = nullptr;
        }
    }
}
