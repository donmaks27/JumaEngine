// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

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

        bool isValid() const { return m_TaskFunction != nullptr; }

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

            virtual void call() = 0;
        };
        template<typename ClassType, typename... Args>
        class ActionTaskFunction_ClassMethod : public ActionTaskFunction
        {
            using class_type = ClassType;
            using function_type = void (class_type::*)(Args...);

        public:
            ActionTaskFunction_ClassMethod(class_type* objectPtr, function_type functionPtr, Args... args)
                : function(functionPtr), arguments(objectPtr, args...)
            {}
            virtual ~ActionTaskFunction_ClassMethod() override = default;

            virtual void call() override
            {
                if (function != nullptr)
                {
                    std::apply(function, arguments);
                }
            }

        private:

            function_type function = nullptr;
            std::tuple<class_type*, Args...> arguments = std::tuple();
        };
        template<typename Function, typename... Args>
        class ActionTaskFunction_Raw : public ActionTaskFunction
        {
            using function_type = Function;

        public:
            ActionTaskFunction_Raw(function_type&& func, Args... args)
                : function(std::forward<function_type>(func)), arguments(args...)
            {}
            virtual ~ActionTaskFunction_Raw() override = default;

            virtual void call() override { std::apply(function, arguments); }

        private:

            function_type function;
            std::tuple<Args...> arguments;
        };

        ActionTaskFunction* m_TaskFunction = nullptr;
    };
}
