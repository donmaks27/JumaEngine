// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "WindowSubsystem_Vulkan_GLFW.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include <GLFW/glfw3.h>

#include "Window_Vulkan_GLFW.h"
#include "engine/Engine.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"

namespace JumaEngine
{
    bool WindowSubsystem_Vulkan_GLFW::initSubsystemInternal()
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

        glfwSetErrorCallback(WindowSubsystem_Vulkan_GLFW::GLFW_ErrorCallback);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        return true;
    }
    void WindowSubsystem_Vulkan_GLFW::GLFW_ErrorCallback(const int errorCode, const char* errorMessage)
    {
        JUMA_LOG(error, JSTR("GLFW error. Code: ") + TO_JSTR(errorCode) + JSTR(". ") + errorMessage);
    }

    void WindowSubsystem_Vulkan_GLFW::clearSubsystemInternal()
    {
        glfwTerminate();

        Super::clearSubsystemInternal();
    }

    jarray<const char*> WindowSubsystem_Vulkan_GLFW::getVulkanInstanceExtensions() const
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

    Window* WindowSubsystem_Vulkan_GLFW::createWindow(const jstring& title, const math::uvector2& size)
    {
        Window_Vulkan_GLFW* window = getOwnerEngine()->createObject<Window_Vulkan_GLFW>();
        if (window == nullptr)
        {
            return nullptr;
        }
        
        RenderSubsystem_Vulkan* renderSubsystem = cast<RenderSubsystem_Vulkan>(getOwnerEngine()->getRenderSubsystem());
        renderSubsystem->registerVulkanObject(window);

        const window_id_type id = m_WindowIDs.getUID();
        if (!window->initWindow(id, title, size))
        {
            delete window;
            return nullptr;
        }

        m_Windows.add(id, window);
        return window;
    }

    Window* WindowSubsystem_Vulkan_GLFW::findWindow(const window_id_type windowID) const
    {
        Window_Vulkan_GLFW* const* window = m_Windows.find(windowID);
        return window != nullptr ? *window : nullptr;
    }

    void WindowSubsystem_Vulkan_GLFW::destroyWindow(const window_id_type windowID)
    {
        Window_Vulkan_GLFW** windowPtr = m_Windows.find(windowID);
        Window_Vulkan_GLFW* window = windowPtr != nullptr ? *windowPtr : nullptr;
        if (window != nullptr)
        {
            window->destroy();
            delete window;
            m_Windows.remove(windowID);
        }
    }
}

#endif
