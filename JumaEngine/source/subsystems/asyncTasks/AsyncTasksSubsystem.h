// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"

#include <thread>
#include <shared_mutex>

#include "ActionTask.h"
#include "jutils/jlist.h"
#include "jutils/juid.h"

namespace JumaEngine
{
    class AsyncTasksSubsystem : public SubsystemBase
    {
        JUMAENGINE_CLASS(AsyncTasksSubsystem, SubsystemBase);

    public:
        AsyncTasksSubsystem() = default;
        virtual ~AsyncTasksSubsystem() override = default;

        bool startTask(ActionTask&& task);

    protected:

        virtual void clearSubsystemInternal() override;

    private:

        struct AsyncTask
        {
            AsyncTask() = default;
            AsyncTask(AsyncTask&& task) noexcept
                : thread(std::move(task.thread))
            {}
            AsyncTask(const AsyncTask&) = delete;

            AsyncTask& operator=(AsyncTask&& task) noexcept
            {
                thread = std::move(task.thread);
                return *this;
            }
            AsyncTask& operator=(const AsyncTask&) = delete;

            std::thread thread;
        };

        jlist<std::thread> m_AsyncTasks;
        mutable std::shared_mutex m_AsyncTasksMutex;


        void startAyncTaskInternal(ActionTask task);
    };
}
