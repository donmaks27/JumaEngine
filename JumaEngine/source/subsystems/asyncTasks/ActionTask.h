// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include <atomic>

namespace JumaEngine
{
    class ActionTaskResultBase
    {
    public:
        ActionTaskResultBase() = default;
        ~ActionTaskResultBase() = default;

        bool isTaskFinished() const { return m_TaskFinished; }
        void markAsFinished() { m_TaskFinished = true; }
        void waitForTaskFinished() const
        {
            while (!m_TaskFinished) {}
        }

        bool isHandled() const { return m_Handled; }
        void markAsHandled() const
        {
            if (isTaskFinished())
            {
                m_Handled = true;
            }
        }

    private:

        std::atomic_bool m_TaskFinished = false;
        mutable std::atomic_bool m_Handled = false;
    };
    template<typename ResultType>
    class ActionTaskResult : public ActionTaskResultBase
    {
    public:
        using result_type = ResultType;

        ActionTaskResult() = default;
        ~ActionTaskResult() = default;

        const result_type& get() const { return m_Result; }
        void set(result_type&& value) { m_Result = std::forward<result_type>(value); }

    private:

        result_type m_Result;
    };
    template<>
    class ActionTaskResult<void> : public ActionTaskResultBase
    {
    public:
        ActionTaskResult() = default;
        ~ActionTaskResult() = default;
    };

    class ActionTask final
    {
    public:
        ActionTask() = default;
        ActionTask(bool shouldMarkHandledAfterFinish)
            : m_ShouldHandleAfterFinish(shouldMarkHandledAfterFinish)
        {}
        ActionTask(const ActionTask&) = delete;
        ActionTask(ActionTask&& task) noexcept
            : m_TaskFunction(task.m_TaskFunction)
            , m_ShouldHandleAfterFinish(task.m_ShouldHandleAfterFinish)
        {
            task.m_TaskFunction = nullptr;
        }
        ~ActionTask()
        {
            clear();
        }

        ActionTask& operator=(const ActionTask&) = delete;
        ActionTask& operator=(ActionTask&& task) noexcept
        {
            clear();
            m_ShouldHandleAfterFinish = task.m_ShouldHandleAfterFinish;
            m_TaskFunction = task.m_TaskFunction;
            task.m_TaskFunction = nullptr;
            return *this;
        }

        template<typename Function, typename... Args>
        const ActionTaskResult<std::invoke_result_t<Function, Args...>>* bindRaw(Function function, Args&&... args)
        {
            clear();
            auto* taskFunction = new ActionTaskFunction_Raw(std::forward<Function>(function), std::forward<Args>(args)...);
            m_TaskFunction = taskFunction;
            return taskFunction->getResultObject();
        }
        template<typename ClassType, typename ResultType, typename... Args>
        const ActionTaskResult<ResultType>* bindClassMethod(ClassType* object, ResultType (ClassType::*function)(Args...), Args... args)
        {
            return this->bindRaw(function, object, args...);
        }
        template<typename ClassType, typename ResultType, typename... Args>
        const ActionTaskResult<ResultType>* bindClassMethod(ClassType* object, ResultType (ClassType::*function)(Args...) const, Args... args)
        {
            return this->bindRaw(function, object, args...);
        }

        bool isValid() const { return m_TaskFunction != nullptr; }
        const ActionTaskResultBase* getTaskResult() const { return isValid() ? m_TaskFunction->getResultObjectBase() : nullptr; }
        bool isFinished() const { return isValid() && m_TaskFunction->isFinished(); }
        bool isResultHandled() const { return isValid() && m_TaskFunction->isResultHandled(); }

        bool execute()
        {
            if (!isValid() || m_TaskFunction->isFinished())
            {
                return false;
            }
            m_TaskFunction->call();
            if (m_ShouldHandleAfterFinish)
            {
                m_TaskFunction->getResultObjectBase()->markAsHandled();
            }
            return true;
        }

        void clear()
        {
            if (isValid())
            {
                delete m_TaskFunction;
                m_TaskFunction = nullptr;
            }
        }

    private:

        class ActionTaskFunction
        {
        public:
            ActionTaskFunction() = default;
            virtual ~ActionTaskFunction() = default;

            virtual const ActionTaskResultBase* getResultObjectBase() const = 0;
            virtual void call() = 0;

            bool isFinished() const { return getResultObjectBase()->isTaskFinished(); }
            bool isResultHandled() const { return getResultObjectBase()->isHandled(); }
        };
        template<typename Function, typename... Args>
        class ActionTaskFunction_Raw : public ActionTaskFunction
        {
            using function_type = Function;
            using result_type = std::invoke_result_t<function_type, Args...>;
            using result_object_type = ActionTaskResult<result_type>;

        public:
            ActionTaskFunction_Raw(function_type func, Args... args)
                : function(std::forward<function_type>(func)), arguments(std::forward<Args>(args)...)
            {}
            virtual ~ActionTaskFunction_Raw() override = default;
            
            const result_object_type* getResultObject() const { return &result; }
            virtual const ActionTaskResultBase* getResultObjectBase() const override { return getResultObject(); }
            virtual void call() override
            {
                callInternal<result_type>();
                result.markAsFinished();
            }

        private:

            function_type function;
            std::tuple<Args...> arguments;
            result_object_type result;


            template<typename Type>
            void callInternal() { result.set(std::apply(function, arguments)); }
            template<>
            void callInternal<void>() { std::apply(function, arguments); }
        };

        ActionTaskFunction* m_TaskFunction = nullptr;
        bool m_ShouldHandleAfterFinish = false;
    };
}
