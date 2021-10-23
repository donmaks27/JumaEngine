// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL) && defined(JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW)

#include "render/window/WindowDescriptionGLFWOld.h"
#include "RenderManagerBase_OpenGL.h"

namespace JumaEngine
{
    class RenderManager_OpenGL_GLFW final : public RenderManagerBase_OpenGL
    {
        JUMAENGINE_CLASS(RenderManager_OpenGL_GLFW, RenderManagerBase_OpenGL)

    public:
        RenderManager_OpenGL_GLFW() = default;
        virtual ~RenderManager_OpenGL_GLFW() override;

        virtual bool shouldCloseWindow(window_id windowID) const override;
        virtual void markWindowShouldClose(window_id windowID) override;
        
        virtual void render(window_id windowID, const RenderParams& renderParams) override;

    protected:

        virtual bool initInternal() override;
        virtual void terminateInternal() override;
        
        virtual WindowDescriptionBaseOld* createWindowInternal(const glm::uvec2& size, const jstring& title) override;

#ifndef JUMAENGINE_SINGLE_WINDOW
        virtual void destroyWindowInternal(window_id windowID) override;
#endif

        virtual void setActiveWindowInCurrentThread(window_id windowID) override;
        
        virtual bool updateWindowSize(window_id windowID, const glm::uvec2& size) override;
        virtual bool updateWindowTitle(window_id windowID, const jstring& title) override;

    private:

        void terminateGLFW();

        static void errorCallback(int code, const char* errorMessage);

        WindowDescriptionGLFWOld* getWindowDescriptionGLFW(const window_id windowID) { return getWindowDescription<WindowDescriptionGLFWOld>(windowID); }
        const WindowDescriptionGLFWOld* getWindowDescriptionGLFW(const window_id windowID) const { return getWindowDescription<WindowDescriptionGLFWOld>(windowID); }
        GLFWwindow* getWindowGLFW(const window_id windowID) const
        {
            const WindowDescriptionGLFWOld* description = getWindowDescriptionGLFW(windowID);
            return description != nullptr ? description->windowPtr : nullptr;
        }
    };
}

#endif
