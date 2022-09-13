// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"

#include <JumaRE/RenderEngine.h>

namespace JumaEngine
{
    class EngineContextObject;

    class EngineBase
    {
    public:
        EngineBase() = default;
        virtual ~EngineBase() = default;

        bool init();
        virtual bool update() = 0;
        void destroy();

        JumaRE::RenderEngine* getRenderEngine() const { return m_RenderEngine; }

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<EngineContextObject, T>)>
        T* createObject() { return this->registerObject(new T()); }
        template<typename T, TEMPLATE_ENABLE(is_base<EngineContextObject, T>)>
        T* registerObject(T* object)
        {
            this->registerObjectInternal(object);
            return object;
        }

    protected:

        JumaRE::RenderEngine* m_RenderEngine = nullptr;


        virtual bool initInternal() { return true; }

        virtual void destroyInternal() {}

    private:

        void registerObjectInternal(EngineContextObject* object);
    };
}
