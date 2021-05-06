// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    template<typename... ArgTypes>
    class jdelegate
    {
    public:
        jdelegate() = default;
        jdelegate(const jdelegate& otherDelegate)
        {
            if (otherDelegate.m_Container != nullptr)
            {
                m_Container = otherDelegate.m_Container->copy();
            }
        }
        jdelegate(jdelegate&& otherDelegate) noexcept
        {
            m_Container = otherDelegate.m_Container;
            otherDelegate.m_Container = nullptr;
        }
        ~jdelegate()
        {
            clear();
        }

        jdelegate& operator=(const jdelegate& otherDelegate)
        {
            if (&otherDelegate != this)
            {
                clear();
                if (otherDelegate.m_Container != nullptr)
                {
                    m_Container = otherDelegate.m_Container->copy();
                }
            }
            return *this;
        }
        jdelegate& operator=(jdelegate&& otherDelegate) noexcept
        {
            clear();
            m_Container = otherDelegate.m_Container;
            otherDelegate.m_Container = nullptr;
            return *this;
        }

        template<typename T>
        void bind(T* object, void (T::*function)(ArgTypes...))
        {
            clear();
            if (object != nullptr)
            {
                m_Container = new delegate_container_impl<T>(object, function);
            }
        }

        template<typename T>
        bool isBinded(T* object, void (T::*function)(ArgTypes...)) const
        {
            if ((object != nullptr) && (m_Container != nullptr))
            {
                const delegate_container_impl<T>* container = dynamic_cast<const delegate_container_impl<T>*>(m_Container);
                if (container != nullptr)
                {
                    return container->isBinded(object, function);
                }
            }
            return false;
        }
        template<typename T>
        bool isBinded(T* object) const
        {
            if ((object != nullptr) && (m_Container != nullptr))
            {
                const delegate_container_impl<T>* container = dynamic_cast<const delegate_container_impl<T>*>(m_Container);
                if (container != nullptr)
                {
                    return container->isBinded(object);
                }
            }
            return false;
        }

        void clear()
        {
            if (m_Container != nullptr)
            {
                delete m_Container;
                m_Container = nullptr;
            }
        }

        void call(ArgTypes... args)
        {
            if (m_Container != nullptr)
            {
                m_Container->call(args...);
            }
        }

    private:

        class delegate_container
        {
        public:
            virtual ~delegate_container() = default;

            virtual delegate_container* copy() = 0;

            virtual void call(ArgTypes...) = 0;
        };
        template<typename T>
        class delegate_container_impl : public delegate_container
        {
            typedef void (T::*function_type)(ArgTypes...);

        public:
            delegate_container_impl(T* object, function_type function)
                : m_Object(object)
                , m_Function(function)
            {}

            virtual delegate_container* copy() override { return new delegate_container_impl(m_Object, m_Function); }

            bool isBinded(T* object, function_type function) const { return isBinded(object) && (m_Function == function); }
            bool isBinded(T* object) const { return m_Object == object; }

            virtual void call(ArgTypes... args) override
            {
                if (m_Object != nullptr)
                {
                    (m_Object->*m_Function)(args...);
                }
            }

        private:

            T* m_Object;
            function_type m_Function;
        };

        delegate_container* m_Container = nullptr;
    };
}
