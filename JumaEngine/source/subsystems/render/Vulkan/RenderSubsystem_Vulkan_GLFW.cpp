// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem_Vulkan_GLFW.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "utils/jlog.h"
#include "WindowDescription_Vulkan_GLFW.h"

namespace JumaEngine
{
    bool RenderSubsystem_Vulkan_GLFW::initSubsystemInternal()
    {
        const int initResult = glfwInit();
        if (initResult == GLFW_FALSE)
        {
            const char* errorStr = nullptr;
            glfwGetError(&errorStr);
            JUMA_LOG(error, errorStr);
            return false;
        }

        glfwSetErrorCallback(RenderSubsystem_Vulkan_GLFW::GLFW_ErrorCallback);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        if (!Super::initSubsystemInternal())
        {
            terminateGLFW();
            return false;
        }
        return true;
    }
    void RenderSubsystem_Vulkan_GLFW::GLFW_ErrorCallback(int code, const char* errorMessage)
    {
        JUMA_LOG(error, JSTR("Code: ") + TO_JSTR(code) + JSTR(". ") + errorMessage);
    }

    void RenderSubsystem_Vulkan_GLFW::clearSubsystemInternal()
    {
        Super::clearSubsystemInternal();

        terminateGLFW();
    }
    void RenderSubsystem_Vulkan_GLFW::terminateGLFW()
    {
        glfwTerminate();
    }

    jarray<const char*> RenderSubsystem_Vulkan_GLFW::getRequiredVulkanExtensions() const
    {
        jarray<const char*> result = Super::getRequiredVulkanExtensions();

        uint32 glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (uint32 index = 0; index < glfwExtensionCount; index++)
        {
            result.add(glfwExtensions[index]);
        }
        return result;
    }

    WindowDescription* RenderSubsystem_Vulkan_GLFW::createWindowInternal(const glm::uvec2& size, const jstring& title)
    {
        GLFWwindow* window = glfwCreateWindow(size.x, size.y, *title, nullptr, nullptr);
        if (window == nullptr)
        {
            JUMA_LOG(warning, JSTR("Failed to create GLFW window"));
            return nullptr;
        }
        glfwSetFramebufferSizeCallback(window, GLFW_FramebufferResizeCallback);

        VkSurfaceKHR surface = nullptr;
        const VkResult result = glfwCreateWindowSurface(getVulkanInstance(), window, nullptr, &surface);
        if (result != VK_SUCCESS)
        {
            JUMA_LOG(warning, JSTR("Failed to create window surface"));
            return nullptr;
        }

        WindowDescription_Vulkan_GLFW* windowDescription = new WindowDescription_Vulkan_GLFW();
        windowDescription->size = size;
        windowDescription->title = title;
        windowDescription->window = window;
        windowDescription->surface = surface;
        glfwSetWindowUserPointer(window, windowDescription);
        return windowDescription;
    }
    void RenderSubsystem_Vulkan_GLFW::terminateWindowInternal(WindowDescription* window)
    {
        WindowDescription_Vulkan_GLFW* window_GLFW = castWindow<WindowDescription_Vulkan_GLFW>(window);
        if ((window_GLFW != nullptr) && (window_GLFW->window != nullptr))
        {
            if (window_GLFW->surface != nullptr)
            {
                vkDestroySurfaceKHR(getVulkanInstance(), window_GLFW->surface, nullptr);
                window_GLFW->surface = nullptr;
            }

            glfwDestroyWindow(window_GLFW->window);
            window_GLFW->window = nullptr;
        }
    }

    void RenderSubsystem_Vulkan_GLFW::setWindowSize(WindowDescription* window, const glm::uvec2& size)
    {
        if (window != nullptr)
        {
            setWindowSizeInternal(window, size);
        }
    }
    void RenderSubsystem_Vulkan_GLFW::GLFW_FramebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        WindowDescription* windowDescription = (WindowDescription*)glfwGetWindowUserPointer(window);
        windowDescription->size = { width, height };
        windowDescription->onSizeChanged.call(windowDescription);
    }

    bool RenderSubsystem_Vulkan_GLFW::shouldCloseWindowInternal(WindowDescription* window) const
    {
        WindowDescription_Vulkan_GLFW* window_GLFW = castWindow<WindowDescription_Vulkan_GLFW>(window);
        if ((window_GLFW != nullptr) && (window_GLFW->window != nullptr))
        {
            return glfwWindowShouldClose(window_GLFW->window) != GLFW_FALSE;
        }
        return false;
    }
    void RenderSubsystem_Vulkan_GLFW::setWindowSizeInternal(WindowDescription* window, const glm::uvec2& size)
    {
        WindowDescription_Vulkan_GLFW* window_GLFW = castWindow<WindowDescription_Vulkan_GLFW>(window);
        if ((window_GLFW != nullptr) && (window_GLFW->window != nullptr))
        {
            glfwSetWindowSize(window_GLFW->window, size.x, size.y);
        }
    }
    void RenderSubsystem_Vulkan_GLFW::setWindowTitleInternal(WindowDescription* window, const jstring& title)
    {
        WindowDescription_Vulkan_GLFW* window_GLFW = castWindow<WindowDescription_Vulkan_GLFW>(window);
        if ((window_GLFW != nullptr) && (window_GLFW->window != nullptr))
        {
            glfwSetWindowTitle(window_GLFW->window, *title);
        }
    }

    void RenderSubsystem_Vulkan_GLFW::render()
    {
        Super::render();
        glfwPollEvents();
    }
}

#endif
