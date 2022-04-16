﻿// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "WindowSubsystem_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "engine/Engine.h"

namespace JumaEngine
{
    void WindowSubsystem_RenderAPIObject_OpenGL::startWindowThread(WindowData_OpenGL* windowData, bool mainWindow)
    {
        if (mainWindow)
        {
            initWindowThread(windowData);
        }
        else
        {
            windowData->windowAsyncTaskID = getParent()->getOwnerEngine()->getAsyncTasksSubsystem()->startTask(
                ActionTask(this, &WindowSubsystem_RenderAPIObject_OpenGL::windowThreadLoop, windowData)
            );
        }
    }
    void WindowSubsystem_RenderAPIObject_OpenGL::windowThreadLoop(WindowData_OpenGL* windowData)
    {
        initWindowThread(windowData);

        while (true)
        {
            ActionTask* currentTask = nullptr;
            {
                std::shared_lock lock(windowData->windowTasksMutex);
                for (auto& task : windowData->windowTasks)
                {
                    if (!task.isStarted())
                    {
                        currentTask = &task;
                        break;
                    }
                }
                if ((currentTask == nullptr) && windowData->shouldExitFromWindowThread)
                {
                    break;
                }
            }

            if (currentTask != nullptr)
            {
                currentTask->execute();
            }
        }

        finishWindowThread(windowData);
    }
    void WindowSubsystem_RenderAPIObject_OpenGL::initWindowThread(WindowData_OpenGL* windowData)
    {
        const GLenum glewInitResult = glewInit();
        if (glewInitResult != GLEW_OK)
        {
            JUMA_LOG(error, reinterpret_cast<const char*>(glewGetErrorString(glewInitResult)));
        }
    }

    const ActionTask* WindowSubsystem_RenderAPIObject_OpenGL::submitTaskForWindow(const window_id_type windowID, ActionTask&& task)
    {
        WindowDescription_OpenGL* description = reinterpret_cast<WindowDescription_OpenGL*>(findWindow(windowID));
        WindowData_OpenGL* data = description != nullptr ? description->windowData : nullptr;
        if (data == nullptr)
        {
            return nullptr;
        }

        if (data->windowAsyncTaskID == INVALID_ASYNC_TASK_ID)
        {
            task.execute();
            return nullptr;
        }

        std::lock_guard lock(data->windowTasksMutex);
        data->windowTasks.removeByPredicate([](const ActionTask& task){ return task.isFinished(); });
        return &data->windowTasks.add(std::forward<ActionTask>(task));
    }
}

#endif
