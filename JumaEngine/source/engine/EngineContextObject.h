// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/type_checks.h"
#include "EngineObjectClass.h"

namespace JumaEngine
{
    class Engine;
    class RenderManagerBase;

    class EngineContextObject
    {
        DECLARE_JUMAENGINE_OBJECT_CLASS(EngineContextObject, EngineObjectClass)

        friend Engine; 

    public:
        EngineContextObject() = default;
        virtual ~EngineContextObject() = default;

        Engine* getOwnerEngine() const { return m_OwnerEngine; }
        RenderManagerBase* getRenderManager() const;

        bool isMainThread() const;

        template<typename To, typename From, TEMPLATE_ENABLE(std::is_base_of_v<EngineContextObject, To> && is_base_and_not_same<From, To>)>
        static To* cast(From* object) { return object != nullptr ? dynamic_cast<To*>(object) : nullptr; }
        template<typename To, typename From, TEMPLATE_ENABLE(std::is_base_of_v<EngineContextObject, To> && is_base_and_not_same<From, To>)>
        static const To* cast(const From* object) { return cast<const To>(object); }

    protected:

    	virtual void onRegister() {}

    private:

        Engine* m_OwnerEngine = nullptr;
    };
}
