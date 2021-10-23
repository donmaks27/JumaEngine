// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "subsystems/render/OpenGL/RenderSubsystem_OpenGL.h"
#include "subsystems/render/window/WindowDescription_GLFW.h"

namespace JumaEngine
{
    class RenderSubsystem_OpenGL_GLFW : public RenderSubsystem_OpenGL
    {
        JUMAENGINE_CLASS(RenderSubsystem_OpenGL_GLFW, RenderSubsystem_OpenGL)

    public:
        RenderSubsystem_OpenGL_GLFW() = default;
        virtual ~RenderSubsystem_OpenGL_GLFW() override = default;

    protected:

        virtual bool initSubsystem() override;
        virtual void terminateSubsystem() override;
        
        virtual WindowDescription* createWindowInternal(const glm::uvec2& size, const jstring& title) override;
        virtual void terminateWindowInternal(WindowDescription* windowDescription) override;

    private:

        static void GLFW_ErrorCallback(int code, const char* errorMessage);

        void terminateGLFW();
    };
}

#endif
