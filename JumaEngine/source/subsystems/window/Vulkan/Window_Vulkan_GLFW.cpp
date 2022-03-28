// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Window_Vulkan_GLFW.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include <GLFW/glfw3.h>

#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"

namespace JumaEngine
{
    Window_Vulkan_GLFW::~Window_Vulkan_GLFW()
    {
        destroyWindow_GLFW();
    }

    bool Window_Vulkan_GLFW::initWindow(const window_id_type windowID, const jstring& title, const math::uvector2& size)
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

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        GLFWwindow* window = glfwCreateWindow(static_cast<int>(size.x), static_cast<int>(size.y), *title, nullptr, nullptr);
        if (window == nullptr)
        {
            JUMA_LOG(error, JSTR("Failed to create GLFW window"));
            return false;
        }

        VkSurfaceKHR surface = nullptr;
        const VkResult result = glfwCreateWindowSurface(getRenderSubsystem()->getVulkanInstance(), window, nullptr, &surface);
        if (result != VK_SUCCESS)
        {
            JUMA_VULKAN_ERROR_LOG(JSTR("Failed to create window surface"), result);
            glfwDestroyWindow(window);
            return false;
        }

        fillSupportedPresentModes();

        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, Window_Vulkan_GLFW::GLFW_FramebufferResizeCallback);

        m_WindowID = windowID;
        m_Title = title;
        m_Size = size;
        m_WindowGLFW = window;
        m_VulkanSurface = surface;
        return true;
    }

    void Window_Vulkan_GLFW::GLFW_FramebufferResizeCallback(GLFWwindow* windowGLFW, int width, int height)
    {
        Window_Vulkan_GLFW* window = static_cast<Window_Vulkan_GLFW*>(glfwGetWindowUserPointer(windowGLFW));
        if (window != nullptr)
        {
            window->onWindowResized({ math::max<uint32>(width, 0), math::max<uint32>(height, 0) });
        }
    }
    
    bool Window_Vulkan_GLFW::shouldClose() const
    {
        return isValid_GLFW() && (glfwWindowShouldClose(m_WindowGLFW) != GLFW_FALSE);
    }

    void Window_Vulkan_GLFW::destroy()
    {
        destroyWindow_GLFW();
    }
    void Window_Vulkan_GLFW::destroyWindow_GLFW()
    {
        if (m_WindowGLFW != nullptr)
        {
            destroyWindow_Vulkan();

            glfwDestroyWindow(m_WindowGLFW);
            m_WindowGLFW = nullptr;
        }
    }

    void Window_Vulkan_GLFW::finishRender()
    {
        glfwPollEvents();
    }
}

#endif
