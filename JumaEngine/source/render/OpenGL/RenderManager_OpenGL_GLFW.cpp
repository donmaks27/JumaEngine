// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderManager_OpenGL_GLFW.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL) && defined(JUMAENGINE_USE_WINDOW_LIB_GLFW)

#include "utils/log.h"

namespace JumaEngine
{
    RenderManager_OpenGL_GLFW::~RenderManager_OpenGL_GLFW()
    {
        terminateGLFW();
    }

    bool RenderManager_OpenGL_GLFW::initInternal()
    {
        const int initResult = glfwInit();
        if (initResult == GLFW_FALSE)
        {
            const char* errorStr = nullptr;
            glfwGetError(&errorStr);
            JUMA_LOG(error, errorStr);
            return false;
        }

        glfwSetErrorCallback(RenderManager_OpenGL_GLFW::errorCallback);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 0);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        createWindow(glm::uvec2(800, 600), JTEXT("JumaEngine"));

        if (!Super::initInternal())
        {
            terminateGLFW();
            terminateWindowDescriptions();
            return false;
        }
        return true;
    }
    void RenderManager_OpenGL_GLFW::errorCallback(const int code, const char* errorMessage)
    {
        JUMA_LOG(error, jstring(JTEXT("Code: ")) + TO_JTEXT(code) + JTEXT(". ") + errorMessage);
    }

    void RenderManager_OpenGL_GLFW::terminateInternal()
    {
        terminateGLFW();
        Super::terminateInternal();
    }
    void RenderManager_OpenGL_GLFW::terminateGLFW()
    {
        glfwTerminate();
    }
    
    WindowDescriptionBase* RenderManager_OpenGL_GLFW::createWindowInternal(const glm::uvec2& size, const jstring& title)
    {
        GLFWwindow* window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, getWindowGLFW(getMainWindowID()));
        if (window == nullptr)
        {
            JUMA_LOG(warning, JTEXT("Fail to create GLFW window."));
            return nullptr;
        }

        WindowDescriptionGLFW* windowDescription = new WindowDescriptionGLFW();
        windowDescription->windowSize = size;
        windowDescription->windowTitle = title;
        windowDescription->windowPtr = window;
        return windowDescription;
    }
    
    bool RenderManager_OpenGL_GLFW::shouldCloseWindow(const window_id windowID) const
    {
        GLFWwindow* window = getWindowGLFW(windowID);
        return (window != nullptr) && glfwWindowShouldClose(window);
    }
    void RenderManager_OpenGL_GLFW::markWindowShouldClose(const window_id windowID)
    {
        GLFWwindow* window = getWindowGLFW(windowID);
        if (window != nullptr)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
    void RenderManager_OpenGL_GLFW::destroyWindowInternal(const window_id windowID)
    {
        GLFWwindow* window = getWindowGLFW(windowID);
        if (window != nullptr)
        {
            glfwDestroyWindow(window);
        }
    }

    void RenderManager_OpenGL_GLFW::setActiveWindowInCurrentThread(const window_id windowID)
    {
        GLFWwindow* window = getWindowGLFW(windowID);
        if (window != nullptr)
        {
            glfwMakeContextCurrent(window);
            glfwSwapInterval(0);
        }
        else
        {
            glfwMakeContextCurrent(nullptr);
        }
    }

    bool RenderManager_OpenGL_GLFW::updateWindowSize(const window_id windowID, const glm::uvec2& size)
    {
        GLFWwindow* window = getWindowGLFW(windowID);
        if (window != nullptr)
        {
            glfwSetWindowSize(window, size.x, size.y);
            return true;
        }
        return false;
    }
    bool RenderManager_OpenGL_GLFW::updateWindowTitle(const window_id windowID, const jstring& title)
    {
        GLFWwindow* window = getWindowGLFW(windowID);
        if (window != nullptr)
        {
            glfwSetWindowTitle(window, title.c_str());
            return true;
        }
        return false;
    }

    void RenderManager_OpenGL_GLFW::render(const window_id windowID)
    {
        Super::render(windowID);

        GLFWwindow* window = getWindowGLFW(windowID);
        if (window != nullptr)
        {
            glfwSwapBuffers(window);
        }
        glfwPollEvents();
    }
}

#endif
