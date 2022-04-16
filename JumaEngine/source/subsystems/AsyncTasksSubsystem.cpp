// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "AsyncTasksSubsystem.h"

namespace JumaEngine
{
    void AsyncTasksSubsystem::clearSubsystemInternal()
    {
        SubsystemBase::clearSubsystemInternal();
        
        for (auto& task : m_AsyncTasks)
        {
            task.value.thread.join();
        }
        m_AsyncTasks.clear();
    }

    async_task_id_type AsyncTasksSubsystem::startTask(ActionTask&& task)
    {
        if (!isValid())
        {
            return INVALID_ASYNC_TASK_ID;
        }

        m_AsyncTasksMutex.lock();
        const async_task_id_type asyncTaskID = m_AsyncTaskIDs.getUID();
        if (asyncTaskID != INVALID_ASYNC_TASK_ID)
        {
            m_AsyncTasks[asyncTaskID].thread = std::thread(&AsyncTasksSubsystem::startAyncTaskInternal, this, asyncTaskID, std::forward<ActionTask>(task));
        }
        m_AsyncTasksMutex.unlock();

        return asyncTaskID;
    }
    void AsyncTasksSubsystem::startAyncTaskInternal(const async_task_id_type taskID, ActionTask task)
    {
        task.execute();

        m_AsyncTasksMutex.lock();
        m_AsyncTasks[taskID].finished = true;
        m_AsyncTasksMutex.unlock();
    }

    bool AsyncTasksSubsystem::isTaskFinishedInternal(async_task_id_type taskID, bool* outTaskExists) const
    {
        if (isValid() && (taskID != INVALID_ASYNC_TASK_ID))
        {
            std::shared_lock lock(m_AsyncTasksMutex);
            const AsyncTask* task = m_AsyncTasks.find(taskID);
            if (task != nullptr)
            {
                return task->finished;
            }
        }

        if (outTaskExists != nullptr)
        {
            *outTaskExists = false;
        }
        return false;
    }
    void AsyncTasksSubsystem::waitForTaskFinished(const async_task_id_type taskID) const
    {
        bool taskExists = true;
        while (!isTaskFinishedInternal(taskID, &taskExists) && taskExists) {}
    }
}
