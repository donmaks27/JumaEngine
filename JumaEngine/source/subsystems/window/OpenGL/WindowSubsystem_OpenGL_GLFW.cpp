// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "WindowSubsystem_OpenGL_GLFW.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include <GLFW/glfw3.h>

#include "Window_OpenGL_GLFW.h"
#include "engine/Engine.h"

namespace JumaEngine
{
    bool WindowSubsystem_OpenGL_GLFW::initSubsystemInternal()
    {
        if (!Super::initSubsystemInternal())
        {
            return false;
        }

        if (glfwInit() == GLFW_FALSE)
        {
            const char* errorStr = nullptr;
            glfwGetError(&errorStr);
            JUMA_LOG(error, errorStr);
            return false;
        }

        glfwSetErrorCallback(WindowSubsystem_OpenGL_GLFW::GLFW_ErrorCallback);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        return true;
    }
    void WindowSubsystem_OpenGL_GLFW::GLFW_ErrorCallback(int errorCode, const char* errorMessage)
    {
        JUMA_LOG(error, JSTR("GLFW error. Code: ") + TO_JSTR(errorCode) + JSTR(". ") + errorMessage);
    }

    void WindowSubsystem_OpenGL_GLFW::clearSubsystemInternal()
    {
        glfwTerminate();

        Super::clearSubsystemInternal();
    }

    Window* WindowSubsystem_OpenGL_GLFW::createWindow(const jstring& title, const math::uvector2& size)
    {
        Window_OpenGL_GLFW* window = getOwnerEngine()->createObject<Window_OpenGL_GLFW>();
        if (window == nullptr)
        {
            return nullptr;
        }

        const window_id_type id = m_WindowIDs.getUID();
        if (!window->initWindow(id, title, size))
        {
            delete window;
            return nullptr;
        }

        m_Windows.add(id, window);
        return window;
    }

    Window* WindowSubsystem_OpenGL_GLFW::findWindow(const window_id_type windowID) const
    {
        Window_OpenGL_GLFW* const* window = m_Windows.find(windowID);
        return window != nullptr ? *window : nullptr;
    }

    void WindowSubsystem_OpenGL_GLFW::destroyWindow(window_id_type windowID)
    {
        Window_OpenGL_GLFW** windowPtr = m_Windows.find(windowID);
        Window_OpenGL_GLFW* window = windowPtr != nullptr ? *windowPtr : nullptr;
        if (window != nullptr)
        {
            window->destroy();
            delete window;
            m_Windows.remove(windowID);
        }
    }
}

#endif
