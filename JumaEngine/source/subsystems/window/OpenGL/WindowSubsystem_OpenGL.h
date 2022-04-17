// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/window/WindowSubsystem.h"

#include "jutils/jlist.h"
#include "subsystems/AsyncTasksSubsystem.h"

namespace JumaEngine
{
    struct WindowThreadTask_OpenGL
    {
        WindowThreadTask_OpenGL() = default;
        WindowThreadTask_OpenGL(ActionTask&& task) noexcept
            : task(std::move(task))
        {}
        WindowThreadTask_OpenGL(WindowThreadTask_OpenGL&& task) noexcept
            : task(std::move(task.task)), finished(task.finished.load()), handled(task.handled.load())
        {}
        WindowThreadTask_OpenGL(const WindowThreadTask_OpenGL&) = delete;

        ActionTask task;
        std::atomic_bool finished = false;
        std::atomic_bool handled = false;
    };
    struct WindowData_OpenGL
    {
        window_id_type windowID = INVALID_WINDOW_ID;
        async_task_id_type windowAsyncTaskID = INVALID_ASYNC_TASK_ID;

        std::shared_mutex windowTasksMutex;
        jlist<WindowThreadTask_OpenGL> windowTasks;

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

        WindowThreadTask_OpenGL* submitTaskForWindow(window_id_type windowID, ActionTask&& task);

    protected:

        void startWindowThread(WindowData_OpenGL* windowData, bool mainWindow);
        void windowThreadLoop(WindowData_OpenGL* windowData);
        virtual void initWindowThread(WindowData_OpenGL* windowData);
        virtual void finishWindowThread(WindowData_OpenGL* windowData) {}
    };
}

#endif
