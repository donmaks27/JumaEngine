// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "ActionTaskQueue.h"

namespace JumaEngine
{
    void ActionTaskQueue::startLoop()
    {
        const bool alreadyStatred = m_LoopStarted.exchange(true);
        if (alreadyStatred)
        {
            return;
        }
        m_ShouldFinishLoop = false;

        onLoopStarted();
        while (true)
        {
            if (m_ShouldFinishLoop)
            {
                break;
            }

            ActionTask* nextTask = nullptr;
            {
                std::lock_guard lock(m_TaskQueueMutex);
                for (auto& task : m_TaskQueue)
                {
                    if (!task.isFinished())
                    {
                        nextTask = &task;
                        break;
                    }
                }
            }
            if (nextTask != nullptr)
            {
                nextTask->execute();
            }
        }
        onLoopFinished();

        m_LoopStarted = false;
    }
    void ActionTaskQueue::waitForLoopFinished() const
    {
        while (m_LoopStarted) {}
    }
    void ActionTaskQueue::stopLoop(const bool waitForFinish)
    {
        m_ShouldFinishLoop = true;
        if (waitForFinish)
        {
            waitForLoopFinished();
        }
    }

    void ActionTaskQueue::submitTask(ActionTask&& task)
    {
        std::lock_guard lock(m_TaskQueueMutex);
        m_TaskQueue.removeByPredicate([](const ActionTask& task){ return task.isResultHandled(); });
        m_TaskQueue.add(std::move(task));
    }
}
