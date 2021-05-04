// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/type_traits_macros.h"

namespace JumaEngine
{
    class Engine;
    class RenderManagerBase;

    class EngineContextObject
    {
        friend Engine; 

    protected:
        EngineContextObject() = default;
        virtual ~EngineContextObject() = default;

    public:

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
