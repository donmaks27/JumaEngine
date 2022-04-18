// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include <mutex>

#include "ActionTask.h"
#include "jutils/jlist.h"

namespace JumaEngine
{
    class ActionTaskQueue
    {
    public:
        ActionTaskQueue() = default;
        ActionTaskQueue(const ActionTaskQueue&) = delete;
        ActionTaskQueue(ActionTaskQueue&& taskQueue) noexcept = delete;
        virtual ~ActionTaskQueue()
        {
            stopLoop(true);
        }

        ActionTaskQueue& operator=(const ActionTaskQueue&) = delete;
        ActionTaskQueue& operator=(ActionTaskQueue&& taskQueue) = delete;

        void startLoop();
        void waitForLoopFinished() const;
        void stopLoop(bool waitForFinish = false);

        void submitTask(ActionTask&& task);

    protected:

        virtual void onLoopStarted() {}
        virtual void onLoopFinished() {}

    private:

        std::mutex m_TaskQueueMutex;
        jlist<ActionTask> m_TaskQueue;
        std::atomic_bool m_LoopStarted = false;
        std::atomic_bool m_ShouldFinishLoop = false;
    };
}
