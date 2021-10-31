// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "subsystems/render/OpenGL/RenderSubsystem_OpenGL.h"

namespace JumaEngine
{
    class RenderSubsystem_OpenGL_GLFW : public RenderSubsystem_OpenGL
    {
        JUMAENGINE_CLASS(RenderSubsystem_OpenGL_GLFW, RenderSubsystem_OpenGL)

    public:
        RenderSubsystem_OpenGL_GLFW() = default;
        virtual ~RenderSubsystem_OpenGL_GLFW() override = default;

        virtual void render(const RenderQuery& query) override;

    protected:

        virtual bool initSubsystemInternal() override;
        virtual void clearSubsystemInternal() override;
        
        virtual WindowDescription* createWindowInternal(const glm::uvec2& size, const jstring& title) override;
        virtual void terminateWindowInternal(const jshared_ptr<WindowDescription>& window) override;

        virtual bool shouldCloseWindowInternal(const jshared_ptr<WindowDescription>& window) const override;
        virtual void setWindowSizeInternal(const jshared_ptr<WindowDescription>& window, const glm::uvec2& size) override;
        virtual void setWindowTitleInternal(const jshared_ptr<WindowDescription>& window, const jstring& title) override;

    private:

        static void GLFW_ErrorCallback(int code, const char* errorMessage);

        void terminateGLFW();
    };
}

#endif
