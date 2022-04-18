// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "AsyncTasksSubsystem.h"

namespace JumaEngine
{
    void AsyncTasksSubsystem::clearSubsystemInternal()
    {
        SubsystemBase::clearSubsystemInternal();
        
        for (auto& task : m_AsyncTasks)
        {
            task.join();
        }
        m_AsyncTasks.clear();
    }

    bool AsyncTasksSubsystem::startTask(ActionTask&& task)
    {
        if (!isValid())
        {
            return false;
        }

        m_AsyncTasksMutex.lock();
        m_AsyncTasks.put(&AsyncTasksSubsystem::startAyncTaskInternal, this, std::forward<ActionTask>(task));
        m_AsyncTasksMutex.unlock();
        return true;
    }
    void AsyncTasksSubsystem::startAyncTaskInternal(ActionTask task)
    {
        task.execute();
    }
}
