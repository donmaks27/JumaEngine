// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "WindowSubsystem_OpenGL_GLFW.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include <GLFW/glfw3.h>

namespace JumaEngine
{
    WindowSubsystem_RenderAPIObject_OpenGL_GLFW::~WindowSubsystem_RenderAPIObject_OpenGL_GLFW()
    {
        clearData_GLFW();
    }

    bool WindowSubsystem_RenderAPIObject_OpenGL_GLFW::initInternal()
    {
        if (!Super::initInternal())
        {
            return false;
        }

        if (glfwInit() == GLFW_FALSE)
        {
#ifndef JUTILS_LOG_DISABLED
            const char* errorStr = nullptr;
            glfwGetError(&errorStr);
            JUMA_LOG(error, errorStr);
#endif
            return false;
        }

        glfwSetErrorCallback(WindowSubsystem_RenderAPIObject_OpenGL_GLFW::GLFW_ErrorCallback);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 0);

        if (!createWindowsFromParent())
        {
            JUMA_LOG(error, JSTR("Failed to create windows from parent object"));
            return false;
        }
        return true;
    }
    void WindowSubsystem_RenderAPIObject_OpenGL_GLFW::GLFW_ErrorCallback(int errorCode, const char* errorMessage)
    {
        JUMA_LOG(error, JSTR("GLFW error. Code: ") + TO_JSTR(errorCode) + JSTR(". ") + errorMessage);
    }

    void WindowSubsystem_RenderAPIObject_OpenGL_GLFW::clearData_GLFW()
    {
        for (auto& description : m_Windows)
        {
            destroyWindow_GLFW(description.key, description.value);
        }
        m_Windows.clear();

        glfwTerminate();
    }

    bool WindowSubsystem_RenderAPIObject_OpenGL_GLFW::createWindow(const window_id_type windowID)
    {
        WindowDescription* description = findParentWindow(windowID);
        if (description == nullptr)
        {
            JUMA_LOG(error, JSTR("Failed to get window data"));
            return false;
        }

        GLFWwindow* existingWindow = !m_Windows.isEmpty() ? m_Windows.begin()->value.windowGLFW : nullptr;

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, description->hidden ? GLFW_FALSE : GLFW_TRUE);
        GLFWwindow* window = glfwCreateWindow(
            static_cast<int>(description->size.x), static_cast<int>(description->size.y), 
            *description->title, nullptr, existingWindow
        );
        if (window == nullptr)
        {
            JUMA_LOG(error, JSTR("Failed to create GLFW window"));
            return false;
        }

        description->supportedPresentModes = { RenderPresentMode::VSync, RenderPresentMode::Immediate };

        WindowDescription_OpenGL_GLFW& windowDescription = m_Windows[windowID];
        windowDescription.windowID = windowID;
        windowDescription.windowGLFW = window;
        glfwSetWindowUserPointer(window, &windowDescription);
        glfwSetFramebufferSizeCallback(window, WindowSubsystem_RenderAPIObject_OpenGL_GLFW::GLFW_FramebufferResizeCallback);
        
        const window_id_type prevActiveWindowID = getActiveWindow();
        setWindowActive(windowID);
        glfwSwapInterval(1);
        if (prevActiveWindowID != INVALID_WINDOW_ID)
        {
            setWindowActive(prevActiveWindowID);
        }
        else
        {
            initOpenGL();
        }
        return true;
    }
    void WindowSubsystem_RenderAPIObject_OpenGL_GLFW::GLFW_FramebufferResizeCallback(GLFWwindow* windowGLFW, int width, int height)
    {
        WindowDescription_OpenGL_GLFW* windowDescription = static_cast<WindowDescription_OpenGL_GLFW*>(glfwGetWindowUserPointer(windowGLFW));
        if (windowDescription != nullptr)
        {
            windowDescription->windowSubsystemObject->onWindowResized(windowDescription->windowID, { math::max<uint32>(width, 0), math::max<uint32>(height, 0) });
        }
    }
    
    void WindowSubsystem_RenderAPIObject_OpenGL_GLFW::setWindowActiveInternal(const window_id_type windowID)
    {
        const WindowDescription_OpenGL_GLFW* windowDescription = m_Windows.find(windowID);
        GLFWwindow* window = windowDescription != nullptr ? windowDescription->windowGLFW : nullptr;
        glfwMakeContextCurrent(window);
    }

    void WindowSubsystem_RenderAPIObject_OpenGL_GLFW::destroyWindow(const window_id_type windowID)
    {
        WindowDescription_OpenGL_GLFW* description = m_Windows.find(windowID);
        if (description == nullptr)
        {
            return;
        }

        destroyWindow_GLFW(windowID, *description);
        m_Windows.remove(windowID);
    }
    void WindowSubsystem_RenderAPIObject_OpenGL_GLFW::destroyWindow_GLFW(const window_id_type windowID, WindowDescription_OpenGL_GLFW& description)
    {
        glfwSetWindowUserPointer(description.windowGLFW, nullptr);
        glfwDestroyWindow(description.windowGLFW);
        description.windowGLFW = nullptr;
    }

    bool WindowSubsystem_RenderAPIObject_OpenGL_GLFW::shouldCloseWindow(const window_id_type windowID) const
    {
        const WindowDescription_OpenGL_GLFW* description = m_Windows.find(windowID);
        if ((description == nullptr) || (description->windowGLFW == nullptr))
        {
            JUMA_LOG(warning, JSTR("Can't find window"));
            return false;
        }
        return glfwWindowShouldClose(description->windowGLFW) != GLFW_FALSE;
    }

    void WindowSubsystem_RenderAPIObject_OpenGL_GLFW::onFinishWindowRender(const window_id_type windowID)
    {
        Super::onFinishWindowRender(windowID);

        WindowDescription_OpenGL_GLFW* description = m_Windows.find(windowID);
        if (description != nullptr)
        {
            glfwSwapBuffers(description->windowGLFW);
        }
    }
    void WindowSubsystem_RenderAPIObject_OpenGL_GLFW::onFinishRender()
    {
        Super::onFinishRender();
        
        glfwPollEvents();
    }
}

#endif
