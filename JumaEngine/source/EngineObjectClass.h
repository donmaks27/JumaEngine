// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/type_traits_macros.h"

namespace JumaEngine
{
    class EngineContextObject;

    class EngineObjectClass
    {
    public:
        EngineObjectClass() = default;
        virtual ~EngineObjectClass() = default;

        virtual const char* getClassName() const = 0;

        template<typename T, TEMPLATE_ENABLE(std::is_base_of_v<EngineContextObject, T>)>
        bool isChild() const { return isDerrivedOrEqual(T::getClass()); }

    protected:

        virtual bool isDerrivedOrEqual(const EngineObjectClass* classObject) const { return false; }
    };
}

#define DECLARE_JUMAENGINE_OBJECT_CLASS(ClassName, ParentClassName)                         \
public:                                                                                     \
    static const EngineObjectClass* getClass() \
    { \
        static ClassName##_Class Class; \
        return &Class; \
    }                         \
protected:                                                                                  \
    class ClassName##_Class : public ParentClassName                                        \
    {                                                                                       \
    private:                                                                                \
    public:                                                                                 \
        ClassName##_Class() = default;                                                      \
        virtual ~ClassName##_Class() override = default;                                    \
        virtual const char* getClassName() const override { return #ClassName; }            \
    protected:                                                                              \
        virtual bool isDerrivedOrEqual(const EngineObjectClass* classObject) const override \
        {                                                                                   \
            if (classObject == nullptr) { return false; }                                   \
            if (classObject == ClassName::getClass()) { return true; }                      \
            return ParentClassName::isDerrivedOrEqual(classObject);                         \
        }                                                                                   \
    };                                                                                      \
private:                                                                                    


#define JUMAENGINE_CLASS(ClassName, ParentClassName)                    \
    DECLARE_JUMAENGINE_OBJECT_CLASS(ClassName, ParentClassName##_Class) \
    typedef ParentClassName Super;
