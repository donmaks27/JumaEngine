// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "EngineSubsystem.h"

#include <jutils/jasync_task_queue.h>

namespace JumaEngine
{
    enum class AsyncTaskType : uint8 { Worker, GameThread };

    class AsyncEngineSubsystem final : public EngineSubsystem
    {
        JUMAENGINE_CLASS(AsyncEngineSubsystem, EngineSubsystem)

        friend Engine;

    public:
        AsyncEngineSubsystem() = default;
        virtual ~AsyncEngineSubsystem() override = default;

        bool addTask(AsyncTaskType type, jasync_task* task);

    protected:

        virtual bool initSubsystem() override;
		virtual void clearSubsystem() override;

    private:

        jasync_task_queue<> m_AsyncTaskQueue;
        
        std::mutex m_AsyncTasks_GameThreadMutex;
        jlist<jasync_task*> m_AsyncTasks_GameThread;
        jlist<jasync_task*> m_AsyncTasks_GameThreadTemp;


        void executeGameThreadTasks();
    };
}
