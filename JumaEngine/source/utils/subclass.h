// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "type_traits_macros.h"

namespace JumaEngine
{
    template<typename T>
    class class_type
    {
    };

    template<typename T>
    class subclass
    {
    public:
        subclass() = default;
        template<typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<T, U>)>
        subclass(class_type<U>&& classType) { m_ClassType = new class_type_object_impl<U>; }
        template<typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<T, U>)>
        subclass(const class_type<U>& classType) { m_ClassType = new class_type_object_impl<U>; }
        template<typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<T, U>)>
        subclass(subclass<U>&& subclassType) { m_ClassType = subclassType.copy(); }
        template<typename U, TEMPLATE_ENABLE(is_base_and_not_abstract<T, U>)>
        subclass(const subclass<U>& subclassType) { m_ClassType = subclassType.copy(); }

        ~subclass() { delete m_ClassType; }

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

            virtual class_type_object* copy() const { return new class_type_object_impl(); }
        };

        class_type_object* m_ClassType = nullptr;


        class_type_object* copy() const { return m_ClassType != nullptr ? m_ClassType->copy() : nullptr; }
    };
    
#define CLASS_TYPE(class_name) class_type<class_name>()
#define SUBCLASS(class_base, class_actual) subclass<class_base>(CLASS_TYPE(class_actual))
}
