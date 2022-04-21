// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/window/WindowSubsystem.h"

#include "subsystems/asyncTasks/ActionTask.h"
#include "subsystems/asyncTasks/ActionTaskQueue.h"

namespace JumaEngine
{
    class WindowSubsystem_RenderAPIObject_OpenGL;
    struct WindowDescription_OpenGL;

    class WindowActionTaskQueue_OpenGL final : public ActionTaskQueue
    {
        friend WindowSubsystem_RenderAPIObject_OpenGL;

    public:
        WindowActionTaskQueue_OpenGL() = default;
        virtual ~WindowActionTaskQueue_OpenGL() override = default;

    protected:

        virtual void onLoopStarted() override;
        virtual void onLoopFinished() override;

    private:

        WindowSubsystem_RenderAPIObject_OpenGL* m_WindowSubsystem = nullptr;
        WindowDescription_OpenGL* m_WindowDescription = nullptr;
    };

    struct WindowDescription_OpenGL : WindowDescription_RenderAPI
    {
        WindowActionTaskQueue_OpenGL* windowTaskQueue = nullptr;
    };

    class WindowSubsystem_RenderAPIObject_OpenGL : public WindowSubsystem_RenderAPIObject
    {
        friend WindowActionTaskQueue_OpenGL;

        using Super = WindowSubsystem_RenderAPIObject;

    public:
        WindowSubsystem_RenderAPIObject_OpenGL() = default;
        virtual ~WindowSubsystem_RenderAPIObject_OpenGL() override = default;

        bool submitTaskForWindow(window_id_type windowID, ActionTask&& task);

    protected:

        void onWindowCreated_OpenGL(WindowDescription_OpenGL* windowDescription);
        virtual void initWindowThread(WindowDescription_OpenGL* windowDescription) { initOpenGL(); }
        virtual void finishWindowThread(WindowDescription_OpenGL* windowDescription) {}
        void initOpenGL() const;
        
        void destroyWindow_OpenGL(window_id_type windowID, WindowDescription_OpenGL& description);
    };
}

#endif
