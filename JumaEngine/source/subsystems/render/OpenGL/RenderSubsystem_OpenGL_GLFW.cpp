// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem_OpenGL_GLFW.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "utils/jlog.h"

namespace JumaEngine
{
    bool RenderSubsystem_OpenGL_GLFW::initSubsystem()
    {
        if (!Super::initSubsystem())
        {
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

        glfwSetErrorCallback(RenderSubsystem_OpenGL_GLFW::GLFW_ErrorCallback);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 0);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        createMainWindow();

        if (!initOpenGL())
        {
            terminateMainWindow();
            terminateGLFW();
            return false;
        }
        return true;
    }
    void RenderSubsystem_OpenGL_GLFW::GLFW_ErrorCallback(int code, const char* errorMessage)
    {
        JUMA_LOG(error, jstring(JSTR("Code: ")) + TO_JSTR(code) + JSTR(". ") + errorMessage);
    }

    void RenderSubsystem_OpenGL_GLFW::terminateSubsystem()
    {
        terminateGLFW();
        Super::terminateSubsystem();
    }
    void RenderSubsystem_OpenGL_GLFW::terminateGLFW()
    {
        glfwTerminate();
    }

    WindowDescription* RenderSubsystem_OpenGL_GLFW::createWindowInternal(const glm::uvec2& size, const jstring& title)
    {
        GLFWwindow* window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
        if (window != nullptr)
        {
            JUMA_LOG(warning, JSTR("Fail to create GLFW window"));
            return nullptr;
        }

        WindowDescription_GLFW* windowDescription = new WindowDescription_GLFW();
        windowDescription->size = size;
        windowDescription->title = title;
        windowDescription->window = window;
        return windowDescription;
    }
    void RenderSubsystem_OpenGL_GLFW::terminateWindowInternal(WindowDescription* windowDescription)
    {
        WindowDescription_GLFW* windowDescription_GLFW = windowDescription != nullptr ? dynamic_cast<WindowDescription_GLFW*>(windowDescription) : nullptr;
        if ((windowDescription_GLFW == nullptr) || (windowDescription_GLFW->window == nullptr))
        {
            return;
        }

        glfwDestroyWindow(windowDescription_GLFW->window);
    }
}

#endif
