// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "jutils/type_checks.h"
#include "EngineObjectClass.h"

namespace JumaEngine
{
    class Engine;

    class EngineContextObject
    {
        JUMAENGINE_BASE_CLASS(EngineContextObject)

        friend Engine;

    public:
        EngineContextObject() = default;
        virtual ~EngineContextObject() = default;

        Engine* getOwnerEngine() const { return m_OwnerEngine; }

        template<typename To, typename From, TEMPLATE_ENABLE(std::is_base_of_v<EngineContextObject, To> && is_base_and_not_same<From, To>)>
        static To* cast(From* object) { return object != nullptr ? dynamic_cast<To*>(object) : nullptr; }
        template<typename To, typename From, TEMPLATE_ENABLE(std::is_base_of_v<EngineContextObject, To> && is_base_and_not_same<From, To>)>
        static const To* cast(const From* object) { return cast<const To>(object); }

    protected:

        virtual void onRegistered() {}

    private:

        Engine* m_OwnerEngine = nullptr;
    };
}
