// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "WindowGLFW.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "utils/log.h"

namespace JumaEngine
{
    WindowGLFW::~WindowGLFW()
    {
        terminateInternal();
    }

    void WindowGLFW::initInternal()
    {
        const int initResult = glfwInit();
        if (initResult == GLFW_FALSE)
        {
            const char* errorStr = nullptr;
            glfwGetError(&errorStr);
            JUMA_LOG(error, errorStr);
            return;
        }

        glfwSetErrorCallback(WindowGLFW::errorCallback);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 0);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    	const glm::uvec2 windowSize = getWindowSize();
        m_WindowObject = glfwCreateWindow(windowSize.x, windowSize.y, getWindowTite().c_str(), nullptr, nullptr);
        if (m_WindowObject == nullptr)
        {
            return;
        }
    	
        glfwMakeContextCurrent(m_WindowObject);
        glfwSwapInterval(0);
    }
    void WindowGLFW::errorCallback(int code, const char* errorMessage)
    {
        JUMA_LOG(error, errorMessage);
    }

    void WindowGLFW::termiante()
    {
        terminateInternal();
    }
    void WindowGLFW::terminateInternal()
    {
        if (isInit())
        {
            glfwTerminate();
            m_WindowObject = nullptr;
        }
    }

    void WindowGLFW::onEngineLoopStart()
    {
        m_LastTimeStamp = glfwGetTime();
        m_DeltaTime = 0.0;
    }
    void WindowGLFW::onRenderFinish()
    {
        if (isInit())
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

    bool WindowGLFW::shouldCloseWindow() const
    {
        return isInit() && glfwWindowShouldClose(m_WindowObject);
    }
    
	void WindowGLFW::updateWindowTitle()
    {
    	if (isInit())
        {
            glfwSetWindowTitle(m_WindowObject, getWindowTite().c_str());
        }
    }
    void WindowGLFW::updateWindowSize()
    {
        if (isInit())
        {
            const glm::uvec2 windowSize = getWindowSize();
            glfwSetWindowSize(m_WindowObject, windowSize.x, windowSize.y);
        }
    }
}
