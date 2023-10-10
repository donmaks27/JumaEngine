// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

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

        template<typename T> requires is_base_class<EngineContextObject, T>
        bool isChild() const { return isDerrivedOrEqual(T::getClass()); }

    protected:

        virtual bool isDerrivedOrEqual(const EngineClass* engineClass) const = 0;

        virtual EngineContextObject* createBaseObject() const { return nullptr; }
        template<typename T> requires is_base_class<EngineContextObject, T>
        T* createObject() const
        {
            if constexpr (!std::is_abstract_v<T>)
            {
                return new T();
            }
            return nullptr;
        }
    };

    template<typename T>
    struct _is_engine_object_class
    {
    private:
        template<typename T1> requires is_derived_from_class<EngineClass, typename T1::ClassType>
                                    && is_same_class<T1, typename T1::ClassType::ObjectType>
        static constexpr bool _helper_func(int32) noexcept { return true; }
        template<typename>
        static constexpr bool _helper_func(int8) noexcept { return false; }
    public:
        static constexpr bool value = _helper_func<T>(0);
    };
    template<typename T>
    constexpr bool is_engine_object_class = _is_engine_object_class<T>::value;

    template<typename T>
    class EngineSubclass
    {
    public:

        using Type = T;
        using ClassType = typename Type::ClassType;

        EngineSubclass(ClassType* engineClass = nullptr)
            : m_Class(engineClass)
        {}
        template<typename T1> requires is_base_class<Type, T1>
        EngineSubclass(const EngineSubclass<T1>& subclass)
            : m_Class(subclass.get())
        {}

        EngineSubclass& operator=(ClassType* engineClass)
        {
            m_Class = engineClass;
            return *this;
        }
        template<typename T1> requires is_base_class<Type, T1>
        EngineSubclass& operator=(const EngineSubclass<T1>& subclass)
        {
            if (this != &subclass)
            {
                m_Class = subclass.get();
            }
            return *this;
        }

        ClassType* get() const { return m_Class; }

        ClassType* operator->() { return m_Class; }
        const ClassType* operator->() const { return m_Class; }

    private:

        ClassType* m_Class = nullptr;
    };

    template<typename T1, typename T2> requires is_engine_object_class<T1> && is_engine_object_class<T2>
    bool operator==(const EngineSubclass<T1>& value1, const EngineSubclass<T2>& value2) { return value1.get() == value2.get(); }
    template<typename T1, typename T2> requires is_engine_object_class<T1> && is_base_class<EngineClass, T2>
    bool operator==(const EngineSubclass<T1>& value1, const T2* value2) { return value1.get() == value2; }
    template<typename T> requires is_engine_object_class<T>
    bool operator==(const EngineSubclass<T>& value1, std::nullptr_t) { return value1.get() == nullptr; }
    
    template<typename T1, typename T2> requires is_engine_object_class<T1> && is_engine_object_class<T2>
    bool operator!=(const EngineSubclass<T1>& value1, const EngineSubclass<T2>& value2) { return !(value1 == value2); }
    template<typename T1, typename T2> requires is_engine_object_class<T1> && is_base_class<EngineClass, T2>
    bool operator!=(const EngineSubclass<T1>& value1, const T2* value2) { return !(value1 == value2); }
    template<typename T> requires is_engine_object_class<T>
    bool operator!=(const EngineSubclass<T>& value1, std::nullptr_t) { return !(value1 == nullptr); }

    template<typename T1, typename T2> requires is_engine_object_class<T1> && is_engine_object_class<T2>
    auto operator<=>(const EngineSubclass<T1>& value1, const EngineSubclass<T2>& value2) { return value1.get() <=> value2.get(); }
    template<typename T1, typename T2> requires is_engine_object_class<T1> && is_base_class<EngineClass, T2>
    auto operator<=>(const EngineSubclass<T1>& value1, const T2* value2) { return value1.get() <=> value2; }
}
