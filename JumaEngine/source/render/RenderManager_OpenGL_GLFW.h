// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "WindowDescriptionGLFW.h"
#include "RenderManager_OpenGL.h"

namespace JumaEngine
{
    class RenderManager_OpenGL_GLFW : public RenderManager_OpenGL
    {
    public:
        RenderManager_OpenGL_GLFW() = default;
        virtual ~RenderManager_OpenGL_GLFW() override;
        
        virtual void finishRender() override;

        virtual bool shouldCloseMainWindow() const override;

    protected:

        virtual bool initInternal() override;
        virtual void terminateInternal() override;
        
        virtual WindowDescriptionBase* createWindow(const glm::uvec2& size, const jstring& title) override;

        virtual void setActiveWindowInCurrentThread(window_id_type windowID) override;
        
        virtual bool updateWindowSize(window_id_type windowID, const glm::uvec2& size) override;
        virtual bool updateWindowTitle(window_id_type windowID, const jstring& title) override;

    private:

        typedef RenderManager_OpenGL Super;


        void terminateGLFW();

        static void errorCallback(int code, const char* errorMessage);

        WindowDescriptionGLFW* getWindowDescriptionGLFW(const window_id_type windowID) { return getWindowDescription<WindowDescriptionGLFW>(windowID); }
        const WindowDescriptionGLFW* getWindowDescriptionGLFW(const window_id_type windowID) const { return getWindowDescription<WindowDescriptionGLFW>(windowID); }
        GLFWwindow* getWindowGLFW(const window_id_type windowID) const
        {
            const WindowDescriptionGLFW* description = getWindowDescriptionGLFW(windowID);
            return description != nullptr ? description->windowPtr : nullptr;
        }
    };
}
