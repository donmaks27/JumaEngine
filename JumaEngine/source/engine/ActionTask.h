// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    class ActionTaskResultBase
    {
    public:
        ActionTaskResultBase() = default;
        ~ActionTaskResultBase() = default;

        bool isValid() const { return m_Valid; }
        void validate() { m_Valid = true; }
        void waitForValidation() const
        {
            while (!m_Valid) {}
        }

        bool isHandled() const { return m_Handled; }
        void markAsHandled() const { m_Handled = true; }

    private:

        std::atomic_bool m_Valid = false;
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
        ActionTask(const ActionTask&) = delete;
        ActionTask(ActionTask&& task) noexcept
            : m_TaskFunction(task.m_TaskFunction)
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
            m_TaskFunction = task.m_TaskFunction;
            task.m_TaskFunction = nullptr;
            return *this;
        }

        template<typename ClassType, typename ResultType, typename... Args>
        const ActionTaskResult<ResultType>* bindClassMethod(ClassType* object, ResultType (ClassType::*function)(Args...), Args... args)
        {
            clear();

            auto* taskFunction = new ActionTaskFunction_ClassMethod(object, function, args...);
            m_TaskFunction = taskFunction;
            return taskFunction->getResultObject();
        }
        template<typename Function, typename... Args>
        const ActionTaskResult<std::invoke_result_t<Function, Args...>>* bindRaw(Function&& function, Args... args)
        {
            clear();

            auto* taskFunction = new ActionTaskFunction_Raw(std::forward<Function>(function), args...);
            m_TaskFunction = taskFunction;
            return taskFunction->getResultObject();
        }

        bool isValid() const { return m_TaskFunction != nullptr; }
        const ActionTaskResultBase* getTaskResult() const { return isValid() ? m_TaskFunction->getResultObjectBase() : nullptr; }
        bool isFinished() const
        {
            const ActionTaskResultBase* result = getTaskResult();
            return (result != nullptr) && result->isValid();
        }
        bool isUseless() const
        {
            const ActionTaskResultBase* result = getTaskResult();
            return (result == nullptr) || (result->isValid() && result->isHandled());
        }

        bool execute()
        {
            if (!isValid())
            {
                return false;
            }
            m_TaskFunction->call();
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

            virtual const ActionTaskResultBase* getResultObjectBase() = 0;
            virtual void call() = 0;
        };
        template<typename ClassType, typename ResultType, typename... Args>
        class ActionTaskFunction_ClassMethod : public ActionTaskFunction
        {
            using class_type = ClassType;
            using result_type = ResultType;
            using function_type = result_type (class_type::*)(Args...);
            using result_object_type = ActionTaskResult<result_type>;

        public:
            ActionTaskFunction_ClassMethod(class_type* objectPtr, function_type functionPtr, Args... args)
                : function(functionPtr), arguments(objectPtr, args...)
            {}
            virtual ~ActionTaskFunction_ClassMethod() override = default;

            const result_object_type* getResultObject() const { return &result; }
            virtual const ActionTaskResultBase* getResultObjectBase() override { return getResultObject(); }
            virtual void call() override
            {
                if (function != nullptr)
                {
                    callInternal<result_type>();
                }
                result.validate();
            }

        private:

            function_type function = nullptr;
            std::tuple<class_type*, Args...> arguments = std::tuple();
            result_object_type result;


            template<typename Type>
            void callInternal() { result.set(std::apply(function, arguments)); }
            template<>
            void callInternal<void>() { std::apply(function, arguments); }
        };
        template<typename Function, typename... Args>
        class ActionTaskFunction_Raw : public ActionTaskFunction
        {
            using function_type = Function;
            using result_type = std::invoke_result_t<function_type, Args...>;
            using result_object_type = ActionTaskResult<result_type>;

        public:
            ActionTaskFunction_Raw(function_type&& func, Args... args)
                : function(std::forward<function_type>(func)), arguments(args...)
            {}
            virtual ~ActionTaskFunction_Raw() override = default;
            
            const result_object_type* getResultObject() const { return &result; }
            virtual const ActionTaskResultBase* getResultObjectBase() override { return getResultObject(); }
            virtual void call() override
            {
                result.set(std::apply(function, arguments));
                result.validate();
            }

        private:

            function_type function;
            std::tuple<Args...> arguments;
            result_object_type result;
        };

        ActionTaskFunction* m_TaskFunction = nullptr;
    };
}
