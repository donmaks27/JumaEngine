// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"

#include <thread>
#include <atomic>
#include <shared_mutex>

#include "engine/ActionTask.h"
#include "jutils/jmap.h"
#include "jutils/juid.h"

namespace JumaEngine
{
    using async_task_id_type = uint64;
    constexpr async_task_id_type INVALID_ASYNC_TASK_ID = juid<async_task_id_type>::invalidUID;

    class AsyncTasksSubsystem : public SubsystemBase
    {
        JUMAENGINE_CLASS(AsyncTasksSubsystem, SubsystemBase);

    public:
        AsyncTasksSubsystem() = default;
        virtual ~AsyncTasksSubsystem() override = default;

        async_task_id_type startTask(ActionTask&& task);

        bool isTaskFinished(const async_task_id_type taskID) const { return isTaskFinishedInternal(taskID, nullptr); }
        void waitForTaskFinished(async_task_id_type taskID) const;

    protected:

        virtual void clearSubsystemInternal() override;

    private:

        struct AsyncTask
        {
            AsyncTask() = default;
            AsyncTask(AsyncTask&& task) noexcept
                : thread(std::move(task.thread))
                , finished(task.finished.load())
            {}
            AsyncTask(const AsyncTask&) = delete;

            AsyncTask& operator=(AsyncTask&& task) noexcept
            {
                thread = std::move(task.thread);
                finished = task.finished.load();
                return *this;
            }
            AsyncTask& operator=(const AsyncTask&) = delete;

            std::thread thread;
            std::atomic_bool finished = false;
        };

        juid<async_task_id_type> m_AsyncTaskIDs;
        jmap<async_task_id_type, AsyncTask> m_AsyncTasks;
        mutable std::shared_mutex m_AsyncTasksMutex;


        void startAyncTaskInternal(async_task_id_type taskID, ActionTask task);

        bool isTaskFinishedInternal(async_task_id_type taskID, bool* outTaskExists) const;
    };
}
