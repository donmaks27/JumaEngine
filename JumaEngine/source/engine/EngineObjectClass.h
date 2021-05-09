// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/type_traits_macros.h"

namespace JumaEngine
{
    class Engine;
    class EngineContextObject;

    class EngineObjectClass
    {
        friend Engine;

    public:
        EngineObjectClass() = default;
        virtual ~EngineObjectClass() = default;

        virtual const char* getClassName() const = 0;

        template<typename T, TEMPLATE_ENABLE(std::is_base_of_v<EngineContextObject, T>)>
        bool isChild() const { return isDerrivedOrEqual(T::getClass()); }

    protected:

        virtual EngineContextObject* createObject() const { return nullptr; }
        template<typename T>
        T* createObjectTemplate() const { return create_object_struct<T, !std::is_abstract_v<T>>()(); }

        virtual bool isDerrivedOrEqual(const EngineObjectClass* classObject) const { return false; }

    private:

        template<typename T, bool Test>
        struct create_object_struct
        {
            T* operator()() const { return nullptr; }
        };
        template<typename T>
        struct create_object_struct<T, true>
        {
            T* operator()() const { return new T(); }
        };
    };
}

#define DECLARE_JUMAENGINE_OBJECT_CLASS(ClassName, ParentClassName)                                     \
public:                                                                                                 \
    class ClassName##_Class : public ParentClassName                                                    \
    {                                                                                                   \
    private:                                                                                            \
    public:                                                                                             \
        ClassName##_Class() = default;                                                                  \
        virtual ~ClassName##_Class() override = default;                                                \
        virtual const char* getClassName() const override { return #ClassName; }                        \
    protected:                                                                                          \
        virtual EngineContextObject* createObject() const { return createObjectTemplate<ClassName>(); } \
        virtual bool isDerrivedOrEqual(const EngineObjectClass* classObject) const override             \
        {                                                                                               \
            if (classObject == nullptr) { return false; }                                               \
            if (classObject == ClassName::getClass()) { return true; }                                  \
            return ParentClassName::isDerrivedOrEqual(classObject);                                     \
        }                                                                                               \
    };                                                                                                  \
    static const ClassName##_Class* getClass()                                                          \
    {                                                                                                   \
        static ClassName##_Class Class;                                                                 \
        return &Class;                                                                                  \
    }                                                                                                   \
private:                                                                                    


#define JUMAENGINE_CLASS(ClassName, ParentClassName)                    \
    DECLARE_JUMAENGINE_OBJECT_CLASS(ClassName, ParentClassName##_Class) \
    typedef ParentClassName Super;
