// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "type_traits_macros.h"

namespace JumaEngine
{
    template<typename T>
    class jclass_type
    {
    };

    template<typename T>
    class jsubclass
    {
    public:
        jsubclass() = default;
        jsubclass(nullptr_t)
            : jsubclass()
        {}

        template<typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<T, U>)>
        jsubclass(const jclass_type<U>& classType) { m_ClassType = new class_type_object_impl<U>; }
        template<typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<T, U>)>
        jsubclass(jclass_type<U>&& classType) { m_ClassType = new class_type_object_impl<U>; }

        template<typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<T, U>)>
        jsubclass(const jsubclass<U>& subclassType)
        {
            if (subclassType.m_ClassType != nullptr)
            {
                m_ClassType = subclassType.m_ClassType->copy();
            }
        }
        template<typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<T, U>)>
        jsubclass(jsubclass<U>&& subclassType)
        {
            m_ClassType = subclassType.m_ClassType;
            subclassType.m_ClassType = nullptr;
        }

        ~jsubclass() { clear(); }

        jsubclass& operator=(nullptr_t)
        {
            clear();
            return *this;
        }

        template<typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<T, U>)>
        jsubclass& operator=(const jclass_type<U>& classType)
        {
            clear();
            m_ClassType = new class_type_object_impl<U>;
            return *this;
        }
        template<typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<T, U>)>
        jsubclass& operator=(jclass_type<U>&& classType)
        {
            clear();
            m_ClassType = new class_type_object_impl<U>;
            return *this;
        }

        template<typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<T, U>)>
        jsubclass& operator=(const jsubclass<U>& subclassType)
        {
            if (this != &subclassType)
            {
                clear();
                if (subclassType.m_ClassType != nullptr)
                {
                    m_ClassType = subclassType.m_ClassType->copy();
                }
            }
            return *this;
        }
        template<typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<T, U>)>
        jsubclass& operator=(jsubclass<U>&& subclassType)
        {
            clear();
            m_ClassType = subclassType.m_ClassType;
            subclassType.m_ClassType = nullptr;
            return *this;
        }

        T* createObject() const { return m_ClassType != nullptr ? m_ClassType->createObject() : nullptr; }

    private:

        class class_type_object
        {
        public:
            class_type_object() = default;
            virtual ~class_type_object() = default;

            virtual T* createObject() const = 0;

            virtual class_type_object* copy() const = 0;
        };
        template<typename U>
        class class_type_object_impl : public class_type_object
        {
        public:
            class_type_object_impl() = default;
            virtual ~class_type_object_impl() override = default;

            virtual T* createObject() const override { return new U(); }

            virtual class_type_object* copy() const override { return new class_type_object_impl(); }
        };

        class_type_object* m_ClassType = nullptr;

        void clear()
        {
            delete m_ClassType;
            m_ClassType = nullptr;
        }
    };
    
#define CLASS_TYPE(class_name) jclass_type<class_name>()
#define SUBCLASS(class_base, class_actual) jsubclass<class_base>(CLASS_TYPE(class_actual))
}
