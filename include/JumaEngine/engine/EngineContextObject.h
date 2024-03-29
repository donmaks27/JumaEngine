﻿// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "EngineClass.h"

#include "EngineObjectPtr.h"

namespace JumaEngine
{
    class Engine;
    class EngineContextObject;
    
    JUTILS_CREATE_MULTICAST_DELEGATE1(OnEngineContextObjectEvent, EngineContextObject*, object);

    class EngineContextObject
    {
        friend Engine;
        friend EngineObjectPtrBase;
        
    public:

        class EngineContextObject_Class : public EngineClass
        {
            friend Engine;

        public:

            using ObjectType = EngineContextObject;

            EngineContextObject_Class() = default;
            virtual ~EngineContextObject_Class() override = default;

            virtual jstring getClassName() const override { return JSTR("EngineContextObject"); }

        protected:

            virtual bool isDerrivedOrEqual(const EngineClass* engineClass) const override
            {
                return (engineClass != nullptr) && (engineClass == EngineContextObject::GetClassStatic());
            }
            virtual EngineContextObject* createBaseObject() const override { return createObject<EngineContextObject>(); }
        };
        using ClassType = EngineContextObject_Class;

        static ClassType* GetClassStatic()
        {
            static ClassType engineClass;
            return &engineClass;
        }
        virtual EngineClass* getClass() const = 0;

        EngineContextObject() = default;
        virtual ~EngineContextObject() = default;

        OnEngineContextObjectEvent onDestroying;


        Engine* getEngine() const { return EngineObjectPtrBase::s_Engine; }
        template<typename T> requires is_base_class<Engine, T>
        T* getEngine() const { return dynamic_cast<T*>(getEngine()); }

    protected:

        virtual void onObjectDescriptorDestroying() { onDestroying.call(this); }

    private:

        jdescriptor_table_pointer m_ObjectDescriptor;
    };
}

#define DECLARE_JUMAENGINE_CLASS(ClassName, ParentClassName)                                                    \
public:                                                                                                         \
    class ClassName##_Class : public ParentClassName::ClassType                                                 \
    {                                                                                                           \
		friend JumaEngine::Engine;                                                                              \
    public:                                                                                                     \
        using ObjectType = ClassName;                                                                           \
        ClassName##_Class() = default;                                                                          \
        virtual ~ClassName##_Class() override = default;                                                        \
        virtual jutils::jstring getClassName() const override { return JSTR(#ClassName); }                      \
    protected:                                                                                                  \
        virtual bool isDerrivedOrEqual(const JumaEngine::EngineClass* engineClass) const override               \
        {                                                                                                       \
            if (engineClass == nullptr) { return false; }                                                       \
            if (engineClass == ClassName::GetClassStatic()) { return true; }                                    \
            return ParentClassName::ClassType::isDerrivedOrEqual(engineClass);                                  \
        }                                                                                                       \
        virtual EngineContextObject* createBaseObject() const override { return createObject<ClassName>(); }    \
    };                                                                                                          \
    using ClassType = ClassName##_Class;                                                                        \
    static ClassType* GetClassStatic()                                                                          \
    {                                                                                                           \
        static ClassType engineClass;                                                                           \
        return &engineClass;                                                                                    \
    }                                                                                                           \
    using Super = ParentClassName;

#define JUMAENGINE_CLASS(ClassName, ParentClassName) DECLARE_JUMAENGINE_CLASS(ClassName, ParentClassName)   \
    virtual JumaEngine::EngineClass* getClass() const override { return ClassName::GetClassStatic(); }      \
private:

#define JUMAENGINE_CLASS_ABSTRACT(ClassName, ParentClassName) DECLARE_JUMAENGINE_CLASS(ClassName, ParentClassName)  \
    virtual JumaEngine::EngineClass* getClass() const override = 0;                                                 \
private:
