// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"

#include <JumaRE/RenderEngine.h>

namespace JumaEngine
{
    class EngineContextObject;

    class Engine
    {
    public:
        Engine() = default;
        virtual ~Engine();

        virtual bool update() = 0;
        void clear();

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

        bool createRenderEngine(JumaRE::RenderAPI api, const JumaRE::WindowCreateInfo& mainWindowInfo);
        void destroyRenderEngine();

        virtual void clearInternal();

    private:

        JumaRE::RenderEngine* m_RenderEngine = nullptr;


        void clearData_Engine();

        void registerObjectInternal(EngineContextObject* object);
    };
}
