// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "WindowSubsystem_Vulkan.h"

#include "jutils/jmap.h"

struct GLFWwindow;

namespace JumaEngine
{
    struct WindowDescription_Vulkan_GLFW : WindowDescription_Vulkan
    {
        GLFWwindow* windowGLFW = nullptr;
    };

    class WindowSubsystem_RenderAPIObject_Vulkan_GLFW final : public WindowSubsystem_RenderAPIObject_Vulkan
    {
        using Super = WindowSubsystem_RenderAPIObject_Vulkan;

    public:
        WindowSubsystem_RenderAPIObject_Vulkan_GLFW() = default;
        virtual ~WindowSubsystem_RenderAPIObject_Vulkan_GLFW() override;

        virtual jarray<const char*> getVulkanInstanceExtensions() const override;

        virtual const WindowDescription_RenderAPI* findWindow(const window_id_type windowID) const override { return m_Windows.find(windowID); }

        virtual bool shouldCloseWindow(window_id_type windowID) const override;

    protected:

        virtual bool initInternal() override;

        virtual WindowDescription_RenderAPI* findWindow(const window_id_type windowID) override { return m_Windows.find(windowID); }

        virtual bool createWindow(window_id_type windowID) override;
        virtual void destroyWindow(window_id_type windowID) override;

        virtual void onFinishRender() override;

    private:

        struct WindowUserObject
        {
            WindowSubsystem_RenderAPIObject_Vulkan_GLFW* object = nullptr;
            window_id_type windowID = INVALID_WINDOW_ID;
        };

        jmap<window_id_type, WindowDescription_Vulkan_GLFW> m_Windows;


        static void GLFW_ErrorCallback(int errorCode, const char* errorMessage);
        static void GLFW_FramebufferResizeCallback(GLFWwindow* windowGLFW, int width, int height);

        void clearData_GLFW();
        void destroyWindow_GLFW(window_id_type windowID, WindowDescription_Vulkan_GLFW& description);
    };
}

#endif
