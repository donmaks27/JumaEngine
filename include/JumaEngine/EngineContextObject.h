// Copyright © 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "EngineClass.h"

namespace JumaEngine
{
    class Engine;

    class EngineContextObject
    {
        friend Engine;

    public:

        class EngineContextObject_Class : public EngineClass
        {
        public:
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

        Engine* getEngine() const { return m_Engine; }
        template<typename T, TEMPLATE_ENABLE(is_base<Engine, T>)>
        T* getEngine() const { return dynamic_cast<T*>(getEngine()); }

    private:

        Engine* m_Engine = nullptr;
    };
}

#define DECLARE_JUMAENGINE_CLASS(ClassName, ParentClassName)                                                    \
public:                                                                                                         \
    class ClassName##_Class : public ParentClassName::ClassType                                                 \
    {                                                                                                           \
    public:                                                                                                     \
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
