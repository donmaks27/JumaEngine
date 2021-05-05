// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL) && defined(JUMAENGINE_USE_WINDOW_LIB_GLFW)

#include "render/window/WindowDescriptionGLFW.h"
#include "RenderManagerBase_OpenGL.h"

namespace JumaEngine
{
    class RenderManager_OpenGL_GLFW : public RenderManagerBase_OpenGL
    {
    public:
        RenderManager_OpenGL_GLFW() = default;
        virtual ~RenderManager_OpenGL_GLFW() override;

        virtual bool shouldCloseWindow(window_id windowID) const override;
        virtual void markWindowShouldClose(window_id windowID) override;
        
        virtual void render(window_id windowID) override;

    protected:

        virtual bool initInternal() override;
        virtual void terminateInternal() override;
        
        virtual WindowDescriptionBase* createWindowInternal(const glm::uvec2& size, const jstring& title) override;

        virtual void destroyWindowInternal(window_id windowID) override;

        virtual void setActiveWindowInCurrentThread(window_id windowID) override;
        
        virtual bool updateWindowSize(window_id windowID, const glm::uvec2& size) override;
        virtual bool updateWindowTitle(window_id windowID, const jstring& title) override;

    private:

        typedef RenderManagerBase_OpenGL Super;


        void terminateGLFW();

        static void errorCallback(int code, const char* errorMessage);

        WindowDescriptionGLFW* getWindowDescriptionGLFW(const window_id windowID) { return getWindowDescription<WindowDescriptionGLFW>(windowID); }
        const WindowDescriptionGLFW* getWindowDescriptionGLFW(const window_id windowID) const { return getWindowDescription<WindowDescriptionGLFW>(windowID); }
        GLFWwindow* getWindowGLFW(const window_id windowID) const
        {
            const WindowDescriptionGLFW* description = getWindowDescriptionGLFW(windowID);
            return description != nullptr ? description->windowPtr : nullptr;
        }
    };
}

#endif
