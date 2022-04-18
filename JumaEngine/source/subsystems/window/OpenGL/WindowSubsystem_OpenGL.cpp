// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "WindowSubsystem_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "engine/Engine.h"
#include "subsystems/asyncTasks/AsyncTasksSubsystem.h"

namespace JumaEngine
{
    void WindowActionTaskQueue_OpenGL::onLoopStarted()
    {
        m_WindowSubsystem->initWindowThread(m_WindowDescription);
    }
    void WindowActionTaskQueue_OpenGL::onLoopFinished()
    {
        m_WindowSubsystem->finishWindowThread(m_WindowDescription);
    }

    void WindowSubsystem_RenderAPIObject_OpenGL::createWindow_OpenGL(WindowDescription_OpenGL* windowDescription, bool mainWindow)
    {
        if (mainWindow)
        {
            initWindowThread(windowDescription);
        }
        else
        {
            windowDescription->windowTaskQueue = new WindowActionTaskQueue_OpenGL();
            windowDescription->windowTaskQueue->m_WindowSubsystem = this;
            windowDescription->windowTaskQueue->m_WindowDescription = windowDescription;

            ActionTask task(true);
            task.bindClassMethod(static_cast<ActionTaskQueue*>(windowDescription->windowTaskQueue), &ActionTaskQueue::startLoop);
            getParent()->getOwnerEngine()->getAsyncTasksSubsystem()->startTask(std::move(task));
        }
    }
    void WindowSubsystem_RenderAPIObject_OpenGL::initWindowThread(WindowDescription_OpenGL* windowDescription)
    {
        const GLenum glewInitResult = glewInit();
        if (glewInitResult != GLEW_OK)
        {
            JUMA_LOG(error, reinterpret_cast<const char*>(glewGetErrorString(glewInitResult)));
        }
    }

    void WindowSubsystem_RenderAPIObject_OpenGL::destroyWindow_OpenGL(const window_id_type windowID, WindowDescription_OpenGL& description)
    {
        if (description.windowTaskQueue != nullptr)
        {
            //description.windowTaskQueue->stopLoop(true);
            delete description.windowTaskQueue;
            description.windowTaskQueue = nullptr;
        }
        else
        {
            finishWindowThread(&description);
        }
    }

    bool WindowSubsystem_RenderAPIObject_OpenGL::submitTaskForWindow(const window_id_type windowID, ActionTask&& task)
    {
        WindowDescription_OpenGL* description = reinterpret_cast<WindowDescription_OpenGL*>(findWindow(windowID));
        if (description == nullptr)
        {
            return false;
        }

        if (description->windowTaskQueue == nullptr)
        {
            task.execute();
            // Shouldn't wait for finish
            return false;
        }

        description->windowTaskQueue->submitTask(std::move(task));
        // Should wait for finish
        return true;
    }
}

#endif
