// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "Window_Vulkan.h"

struct GLFWwindow;

namespace JumaEngine
{
    class Window_Vulkan_GLFW : public Window_Vulkan
    {
        JUMAENGINE_CLASS(Window_Vulkan_GLFW, Window_Vulkan)

    public:
        Window_Vulkan_GLFW() = default;
        virtual ~Window_Vulkan_GLFW() override;

        virtual bool isValid() const override { return isValid_GLFW(); }

        bool initWindow(window_id_type windowID, const jstring& title, const math::uvector2& size);

        virtual bool shouldClose() const override;
        virtual void destroy() override;

        virtual void startRender() override {}
        virtual void finishRender() override;

    protected:

        GLFWwindow* m_WindowGLFW = nullptr;

    private:

        bool isValid_GLFW() const { return m_WindowGLFW != nullptr; }

        static void GLFW_FramebufferResizeCallback(GLFWwindow* windowGLFW, int width, int height);

        void destroyWindow_GLFW();
    };
}

#endif
