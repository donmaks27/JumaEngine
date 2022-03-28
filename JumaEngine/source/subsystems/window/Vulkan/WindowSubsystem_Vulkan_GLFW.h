// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "WindowSubsystem_Vulkan.h"

#include "jutils/jmap.h"

namespace JumaEngine
{
    class Window_Vulkan_GLFW;

    class WindowSubsystem_Vulkan_GLFW : public WindowSubsystem_Vulkan
    {
        JUMAENGINE_CLASS(WindowSubsystem_Vulkan_GLFW, WindowSubsystem_Vulkan)

    public:
        WindowSubsystem_Vulkan_GLFW() = default;
        virtual ~WindowSubsystem_Vulkan_GLFW() override = default;

        virtual jarray<const char*> getVulkanInstanceExtensions() const override;

        virtual Window* createWindow(const jstring& title, const math::uvector2& size) override;
        virtual Window* findWindow(window_id_type windowID) const override;
        virtual void destroyWindow(window_id_type windowID) override;

    protected:

        virtual bool initSubsystemInternal() override;
        virtual void clearSubsystemInternal() override;

    private:

        jmap<window_id_type, Window_Vulkan_GLFW*> m_Windows;


        static void GLFW_ErrorCallback(int errorCode, const char* errorMessage);
    };
}

#endif
