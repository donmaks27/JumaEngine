// Copyright 2022 Leonov Maksim. All Rights Reserved.

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
            ActionTask task;
            task.bindClassMethod(this, &WindowSubsystem_RenderAPIObject_OpenGL::windowThreadLoop, windowData);
            windowData->windowAsyncTaskID = getParent()->getOwnerEngine()->getAsyncTasksSubsystem()->startTask(std::move(task));
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
                    if (!task.isFinished())
                    {
                        currentTask = &task;
                        break;
                    }
                }
            }
            if (currentTask != nullptr)
            {
                currentTask->execute();
            }
            else if (windowData->shouldExitFromWindowThread.load())
            {
                break;
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

    bool WindowSubsystem_RenderAPIObject_OpenGL::submitTaskForWindow(const window_id_type windowID, ActionTask&& task)
    {
        WindowDescription_OpenGL* description = reinterpret_cast<WindowDescription_OpenGL*>(findWindow(windowID));
        WindowData_OpenGL* data = description != nullptr ? description->windowData : nullptr;
        if (data == nullptr)
        {
            return false;
        }

        if (data->windowAsyncTaskID == INVALID_ASYNC_TASK_ID)
        {
            task.execute();
            return false;
        }

        std::lock_guard lock(data->windowTasksMutex);
        data->windowTasks.removeByPredicate([](const ActionTask& task){ return task.isUseless(); });
        data->windowTasks.add(std::forward<ActionTask>(task));
        return true;
    }
}

#endif
