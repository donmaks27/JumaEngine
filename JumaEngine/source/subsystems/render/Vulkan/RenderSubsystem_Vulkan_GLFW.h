// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "RenderSubsystem_Vulkan.h"

namespace JumaEngine
{
    class RenderSubsystem_Vulkan_GLFW final : public RenderSubsystem_Vulkan
    {
        JUMAENGINE_CLASS(RenderSubsystem_Vulkan_GLFW, RenderSubsystem_Vulkan)

    public:
        RenderSubsystem_Vulkan_GLFW() = default;
        virtual ~RenderSubsystem_Vulkan_GLFW() override = default;

    protected:

        virtual bool initSubsystemInternal() override;
        virtual void clearSubsystemInternal() override;
        
        virtual jarray<const char*> getRequiredVulkanExtensions() const override;
        
        virtual WindowDescription* createWindowInternal(const glm::uvec2& size, const jstring& title) override;
        virtual void terminateWindowInternal(const jshared_ptr<WindowDescription>& window) override;

        virtual bool shouldCloseWindowInternal(const jshared_ptr<WindowDescription>& window) const override;
        virtual void setWindowSizeInternal(const jshared_ptr<WindowDescription>& window, const glm::uvec2& size) override;
        virtual void setWindowTitleInternal(const jshared_ptr<WindowDescription>& window, const jstring& title) override;
        
        virtual void render(const RenderQuery& query) override;

    private:

        static void GLFW_ErrorCallback(int code, const char* errorMessage);

        void terminateGLFW();
    };
}

#endif
