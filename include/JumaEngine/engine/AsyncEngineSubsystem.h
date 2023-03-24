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

        bool addTask(AsyncTaskType type, std::function<void()> task);

    protected:

        virtual bool initSubsystem() override;
		virtual void clearSubsystem() override;

    private:

        jasync_task_queue<> m_AsyncTaskQueue;
        
        jlist<std::function<void()>> m_AsyncTasks_GameThread;
        std::mutex m_AsyncTasks_GameThreadMutex;


        void executeGameThreadTasks();
    };
}
