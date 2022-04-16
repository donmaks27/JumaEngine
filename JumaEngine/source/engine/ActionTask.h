// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include <atomic>

namespace JumaEngine
{
    class ActionTask final
    {
    public:
        ActionTask() = default;
        template<typename ClassType, typename... Args>
        ActionTask(ClassType* object, void (ClassType::*function)(Args...), Args... args)
            : m_TaskFunction(new ActionTaskFunction_ClassMethod(object, function, args...))
        {}
        template<typename Function, typename... Args>
        ActionTask(Function&& function, Args... args)
            : m_TaskFunction(new ActionTaskFunction_Raw(std::forward<Function>(function), args...))
        {}
        ActionTask(const ActionTask&) = delete;
        ActionTask(ActionTask&& task) noexcept
        {
            if (task.isValid())
            {
                const bool alreadyStarted = task.m_Started.exchange(true);
                if (!alreadyStarted)
                {
                    m_TaskFunction = task.m_TaskFunction;
                    task.m_TaskFunction = nullptr;
                    task.m_Started = false;
                }
            }
        }
        ~ActionTask()
        {
            if (isValid())
            {
                const bool alreadyStarted = m_Started.exchange(true);
                if (alreadyStarted)
                {
                    while(!m_Finished.load()) {}
                }
                delete m_TaskFunction;
            }
        }

        ActionTask& operator=(const ActionTask&) = delete;
        ActionTask& operator=(ActionTask&& task) = delete;

        bool isValid() const { return m_TaskFunction != nullptr; }
        bool isStarted() const { return m_Started; }
        bool isFinished() const { return m_Finished; }

        bool execute()
        {
            if (!isValid())
            {
                return false;
            }

            const bool alreadyStarted = m_Started.exchange(true);
            if (alreadyStarted)
            {
                return false;
            }

            m_TaskFunction->call();
            m_Finished = true;
            return true;
        }

        void waitForFinish() const
        {
            if (isValid())
            {
                while (!m_Finished.load()) {}
            }
        }

    private:

        class ActionTaskFunction
        {
        public:
            ActionTaskFunction() = default;
            virtual ~ActionTaskFunction() = default;

            virtual void call() = 0;
        };
        template<typename ClassType, typename... Args>
        class ActionTaskFunction_ClassMethod : public ActionTaskFunction
        {
            using class_type = ClassType;
            using function_type = void (class_type::*)(Args...);

        public:
            ActionTaskFunction_ClassMethod(class_type* objectPtr, function_type functionPtr, Args... args)
                : object(objectPtr), function(functionPtr), arguments(args...)
            {}
            virtual ~ActionTaskFunction_ClassMethod() override = default;

            virtual void call() override
            {
                if ((object != nullptr) && (function != nullptr))
                {
                    std::apply(function, std::tuple_cat(std::make_tuple(object), arguments));
                }
            }

        private:

            class_type* object = nullptr;
            function_type function = nullptr;
            std::tuple<Args...> arguments = std::tuple();
        };
        template<typename Function, typename... Args>
        class ActionTaskFunction_Raw : public ActionTaskFunction
        {
            using function_type = Function;

        public:
            ActionTaskFunction_Raw(function_type&& func, Args... args)
                : function(std::forward<Function>(func)), arguments(args...)
            {}
            virtual ~ActionTaskFunction_Raw() override = default;

            virtual void call() override { std::apply(function, arguments); }

        private:

            function_type function;
            std::tuple<Args...> arguments;
        };

        ActionTaskFunction* m_TaskFunction = nullptr;
        std::atomic_bool m_Started = false;
        std::atomic_bool m_Finished = false;
    };
}
