// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/window/WindowSubsystem.h"

#include "jutils/jlist.h"
#include "subsystems/AsyncTasksSubsystem.h"

namespace JumaEngine
{
    struct WindowData_OpenGL
    {
        window_id_type windowID = INVALID_WINDOW_ID;
        async_task_id_type windowAsyncTaskID = INVALID_ASYNC_TASK_ID;

        std::shared_mutex windowTasksMutex;
        jlist<ActionTask> windowTasks;

        std::atomic_bool shouldExitFromWindowThread = false;
    };
    struct WindowDescription_OpenGL : WindowDescription_RenderAPI
    {
        WindowData_OpenGL* windowData = nullptr;
    };

    class WindowSubsystem_RenderAPIObject_OpenGL : public WindowSubsystem_RenderAPIObject
    {
        using Super = WindowSubsystem_RenderAPIObject;

    public:
        WindowSubsystem_RenderAPIObject_OpenGL() = default;
        virtual ~WindowSubsystem_RenderAPIObject_OpenGL() override = default;

        bool submitTaskForWindow(window_id_type windowID, ActionTask&& task);

    protected:

        void startWindowThread(WindowData_OpenGL* windowData, bool mainWindow);
        void windowThreadLoop(WindowData_OpenGL* windowData);
        virtual void initWindowThread(WindowData_OpenGL* windowData);
        virtual void finishWindowThread(WindowData_OpenGL* windowData) {}
    };
}

#endif
