// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/engine/AsyncEngineSubsystem.h"

namespace JumaEngine
{
    bool AsyncEngineSubsystem::initSubsystem()
    {
        if (!Super::initSubsystem())
        {
            return false;
        }
        m_AsyncTaskQueue.init(2);
        return true;
    }
    void AsyncEngineSubsystem::clearSubsystem()
    {
        m_AsyncTaskQueue.stop();

        m_AsyncTasks_GameThreadMutex.lock();
        m_AsyncTasks_GameThread.clear();
        m_AsyncTasks_GameThreadMutex.unlock();

        Super::clearSubsystem();
    }

    bool AsyncEngineSubsystem::addTask(const AsyncTaskType type, std::function<void()> task)
    {
        switch (type)
        {
        case AsyncTaskType::Worker: return m_AsyncTaskQueue.createTask<jasync_task_default>(std::move(task));
        case AsyncTaskType::GameThread:
            {
                std::lock_guard lock(m_AsyncTasks_GameThreadMutex);
                m_AsyncTasks_GameThread.add(std::move(task));
            }
            return true;
        default: ;
        }
        return false;
    }

    void JumaEngine::AsyncEngineSubsystem::executeGameThreadTasks()
    {
        m_AsyncTasks_GameThreadMutex.lock();
        jlist<std::function<void()>> tasks = m_AsyncTasks_GameThread;
        m_AsyncTasks_GameThread.clear();
        m_AsyncTasks_GameThreadMutex.unlock();

        for (const auto& task : tasks)
        {
            if (task != nullptr)
            {
                task();
            }
        }
    }
}
