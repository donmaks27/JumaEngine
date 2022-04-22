// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "WindowSubsystem_Vulkan_GLFW.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include <GLFW/glfw3.h>

#include "engine/Engine.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"

namespace JumaEngine
{
    WindowSubsystem_RenderAPIObject_Vulkan_GLFW::~WindowSubsystem_RenderAPIObject_Vulkan_GLFW()
    {
        clearData_GLFW();
    }

    jarray<const char*> WindowSubsystem_RenderAPIObject_Vulkan_GLFW::getVulkanInstanceExtensions() const
    {
        uint32 extensionsCount = 0;
        const char** extenstions = glfwGetRequiredInstanceExtensions(&extensionsCount);
        if (extensionsCount == 0)
        {
            return {};
        }
        jarray<const char*> result(static_cast<int32>(extensionsCount));
        for (int32 index = 0, size = static_cast<int32>(extensionsCount); index < size; index++)
        {
            result[index] = extenstions[index];
        }
        return result;
    }

    bool WindowSubsystem_RenderAPIObject_Vulkan_GLFW::initInternal()
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

        glfwSetErrorCallback(WindowSubsystem_RenderAPIObject_Vulkan_GLFW::GLFW_ErrorCallback);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        if (!createWindowsFromParent())
        {
            JUMA_LOG(error, JSTR("Failed to create windows from parent object"));
            return false;
        }
        return true;
    }
    void WindowSubsystem_RenderAPIObject_Vulkan_GLFW::GLFW_ErrorCallback(int errorCode, const char* errorMessage)
    {
        JUMA_LOG(error, JSTR("GLFW error. Code: ") + TO_JSTR(errorCode) + JSTR(". ") + errorMessage);
    }

    void WindowSubsystem_RenderAPIObject_Vulkan_GLFW::clearData_GLFW()
    {
        for (auto& description : m_Windows)
        {
            destroyWindow_GLFW(description.key, description.value);
        }
        m_Windows.clear();

        glfwTerminate();
    }

    bool WindowSubsystem_RenderAPIObject_Vulkan_GLFW::createWindow(const window_id_type windowID)
    {
        WindowDescription* description = findParentWindow(windowID);
        if (description == nullptr)
        {
            JUMA_LOG(error, JSTR("Failed to get window data"));
            return false;
        }

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        GLFWwindow* window = glfwCreateWindow(static_cast<int>(description->size.x), static_cast<int>(description->size.y), *description->title, nullptr, nullptr);
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

        WindowDescription_Vulkan_GLFW& windowDescription = m_Windows[windowID];
        windowDescription.windowID = windowID;
        windowDescription.windowSubsystemObject = this;
        windowDescription.windowGLFW = window;
        windowDescription.vulkanSurface = surface;
        updateSupportedPresentModes(windowID, windowDescription, *description);

        glfwSetWindowUserPointer(window, &windowDescription);
        glfwSetFramebufferSizeCallback(window, WindowSubsystem_RenderAPIObject_Vulkan_GLFW::GLFW_FramebufferResizeCallback);

        createVulkanSwapchain(windowID, windowDescription, description->size);
        return true;
    }
    void WindowSubsystem_RenderAPIObject_Vulkan_GLFW::GLFW_FramebufferResizeCallback(GLFWwindow* windowGLFW, int width, int height)
    {
        const WindowDescription_Vulkan_GLFW* windowDescription = static_cast<WindowDescription_Vulkan_GLFW*>(glfwGetWindowUserPointer(windowGLFW));
        if (windowDescription != nullptr)
        {
            windowDescription->windowSubsystemObject->onWindowResized(windowDescription->windowID, { math::max<uint32>(width, 0), math::max<uint32>(height, 0) });
        }
    }

    void WindowSubsystem_RenderAPIObject_Vulkan_GLFW::destroyWindow(const window_id_type windowID)
    {
        WindowDescription_Vulkan_GLFW* description = m_Windows.find(windowID);
        if (description == nullptr)
        {
            return;
        }

        destroyWindow_GLFW(windowID, *description);

        m_Windows.remove(windowID);
    }
    void WindowSubsystem_RenderAPIObject_Vulkan_GLFW::destroyWindow_GLFW(const window_id_type windowID, WindowDescription_Vulkan_GLFW& description)
    {
        destroyWindow_Vulkan(windowID, description);

        glfwSetWindowUserPointer(description.windowGLFW, nullptr);
        glfwDestroyWindow(description.windowGLFW);
        description.windowGLFW = nullptr;
    }

    bool WindowSubsystem_RenderAPIObject_Vulkan_GLFW::shouldCloseWindow(const window_id_type windowID) const
    {
        const WindowDescription_Vulkan_GLFW* description = m_Windows.find(windowID);
        if ((description == nullptr) || (description->windowGLFW == nullptr))
        {
            JUMA_LOG(warning, JSTR("Can't find window"));
            return false;
        }
        return glfwWindowShouldClose(description->windowGLFW) != GLFW_FALSE;
    }

    void WindowSubsystem_RenderAPIObject_Vulkan_GLFW::onFinishRender()
    {
        Super::onFinishRender();

        glfwPollEvents();
    }
}

#endif
