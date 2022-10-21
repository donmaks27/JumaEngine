// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"

namespace JumaEngine
{
    class Engine;
    class EngineContextObject;

    class EngineClass
    {
        friend Engine;

    public:
        EngineClass() = default;
        virtual ~EngineClass() = default;

        virtual jstring getClassName() const = 0;

        template<typename T, TEMPLATE_ENABLE(is_base<EngineContextObject, T>)>
        bool isChild() const { return isDerrivedOrEqual(T::getClass()); }

    protected:

        virtual bool isDerrivedOrEqual(const EngineClass* engineClass) const = 0;

        virtual EngineContextObject* createBaseObject() const { return nullptr; }
        template<typename T, TEMPLATE_ENABLE(is_base<EngineContextObject, T>)>
        T* createObject() const { return create_object_struct<T, !std::is_abstract_v<T>>::create(); }

    private:

        template<typename T, bool Test>
        struct create_object_struct
        {
            static T* create() { return nullptr; }
        };
        template<typename T>
        struct create_object_struct<T, true>
        {
            static T* create() { return new T(); }
        };
    };

    template<typename T>
    class EngineSubclass
    {
        static_assert(is_base<EngineContextObject, T>);

    public:

        using Type = T;
        using ClassType = typename Type::ClassType;

        EngineSubclass(ClassType* engineClass = nullptr)
            : m_Class(engineClass)
        {}
        template<typename T1, TEMPLATE_ENABLE(is_base<Type, T1>)>
        EngineSubclass(const EngineSubclass<T1>& subclass)
            : m_Class(subclass.get())
        {}

        EngineSubclass& operator=(ClassType* engineClass)
        {
            m_Class = engineClass;
            return *this;
        }
        template<typename T1, TEMPLATE_ENABLE(is_base<Type, T1>)>
        EngineSubclass& operator=(const EngineSubclass<T1>& subclass)
        {
            if (this != &subclass)
            {
                m_Class = subclass.get();
            }
            return *this;
        }

        ClassType* get() const { return m_Class; }
        operator ClassType*() const { return get(); }
        
        template<typename T1, TEMPLATE_ENABLE(is_base<Type, T1>)>
        bool operator==(const EngineSubclass<T1>& subclass) const { return m_Class == subclass.get(); }
        bool operator==(const ClassType* engineClass) const { return m_Class == engineClass; }
        template<typename T1, TEMPLATE_ENABLE(is_base<Type, T1>)>
        bool operator!=(const EngineSubclass<T1>& subclass) const { return m_Class != subclass.get(); }
        bool operator!=(const ClassType* engineClass) const { return m_Class != engineClass; }
        template<typename T1, TEMPLATE_ENABLE(is_base<Type, T1>)>
        bool operator<(const EngineSubclass<T1>& subclass) const { return m_Class < subclass.get(); }
        bool operator<(const ClassType* engineClass) const { return m_Class < engineClass; }

    private:

        ClassType* m_Class = nullptr;
    };
}
