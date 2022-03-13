// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "jutils/type_checks.h"

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

        virtual jstring getClassName() const = 0;

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

#define DECLARE_JUMAENGINE_CLASS_OBJECT(ClassName, ParentClassName)                                                 \
public:                                                                                                             \
    class ClassName##_Class : public ParentClassName                                                                \
    {                                                                                                               \
        friend Engine;                                                                                              \
    public:                                                                                                         \
        ClassName##_Class() = default;                                                                              \
        virtual ~ClassName##_Class() override = default;                                                            \
        virtual jstring getClassName() const override { return #ClassName; }                                        \
    protected:                                                                                                      \
        virtual EngineContextObject* createObject() const override { return createObjectTemplate<ClassName>(); }    \
        virtual bool isDerrivedOrEqual(const EngineObjectClass* classObject) const override                         \
        {                                                                                                           \
            if (classObject == nullptr) { return false; }                                                           \
            if (classObject == ClassName::getClass()) { return true; }                                              \
            return ParentClassName::isDerrivedOrEqual(classObject);                                                 \
        }                                                                                                           \
    };                                                                                                              \
    typedef ClassName##_Class ClassType;                                                                            \
    static const ClassType* getClass()                                                                              \
    {                                                                                                               \
        static ClassType Class;                                                                                     \
        return &Class;                                                                                              \
    }                                                                                                               \
private:

#define JUMAENGINE_BASE_CLASS(ClassName) DECLARE_JUMAENGINE_CLASS_OBJECT(ClassName, EngineObjectClass)  \
    public: virtual const ClassType* getObjectClass() const = 0;                                        \
    private:

#define JUMAENGINE_ABSTRACT_CLASS(ClassName, ParentClassName)               \
    DECLARE_JUMAENGINE_CLASS_OBJECT(ClassName, ParentClassName##_Class)     \
    typedef ParentClassName Super;                                          \
    public: virtual const ClassType* getObjectClass() const override = 0;   \
    private:

#define JUMAENGINE_CLASS(ClassName, ParentClassName)                                        \
    DECLARE_JUMAENGINE_CLASS_OBJECT(ClassName, ParentClassName##_Class)                     \
    typedef ParentClassName Super;                                                          \
    public: virtual const ClassType* getObjectClass() const override { return getClass(); } \
    private:
