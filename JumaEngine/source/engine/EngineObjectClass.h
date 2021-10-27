// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/type_checks.h"

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



    class EngineOld;
    class EngineContextObjectOld;

    class EngineObjectClassOld
    {
        friend EngineOld;

    public:
        EngineObjectClassOld() = default;
        virtual ~EngineObjectClassOld() = default;

        virtual jstring getClassName() const = 0;

        template<typename T, TEMPLATE_ENABLE(std::is_base_of_v<EngineContextObjectOld, T>)>
        bool isChild() const { return isDerrivedOrEqual(T::getClass()); }

    protected:

        virtual EngineContextObjectOld* createObject() const { return nullptr; }
        template<typename T>
        T* createObjectTemplate() const { return create_object_struct<T, !std::is_abstract_v<T>>()(); }

        virtual bool isDerrivedOrEqual(const EngineObjectClassOld* classObject) const { return false; }

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

#define JUMAENGINE_BASE_CLASS(ClassName) DECLARE_JUMAENGINE_CLASS_OBJECT(ClassName, EngineObjectClass)
#define JUMAENGINE_CLASS(ClassName, ParentClassName)                    \
    DECLARE_JUMAENGINE_CLASS_OBJECT(ClassName, ParentClassName##_Class) \
    typedef ParentClassName Super;



#define DECLARE_JUMAENGINE_OBJECT_CLASS_OLD(ClassName, ParentClassName)                                             \
public:                                                                                                             \
    class ClassName##_Class : public ParentClassName                                                                \
    {                                                                                                               \
        friend EngineOld;                                                                                              \
    public:                                                                                                         \
        ClassName##_Class() = default;                                                                              \
        virtual ~ClassName##_Class() override = default;                                                            \
        virtual jstring getClassName() const override { return #ClassName; }                                        \
    protected:                                                                                                      \
        virtual EngineContextObjectOld* createObject() const override { return createObjectTemplate<ClassName>(); }    \
        virtual bool isDerrivedOrEqual(const EngineObjectClassOld* classObject) const override                      \
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


#define JUMAENGINE_CLASS_OLD(ClassName, ParentClassName)                    \
    DECLARE_JUMAENGINE_OBJECT_CLASS_OLD(ClassName, ParentClassName##_Class) \
    typedef ParentClassName Super;
