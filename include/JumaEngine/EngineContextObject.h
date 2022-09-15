// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"

namespace JumaEngine
{
    class Engine;

    class EngineContextObject
    {
        friend Engine;

    public:
        EngineContextObject() = default;
        virtual ~EngineContextObject() = default;

        Engine* getEngine() const { return m_Engine; }
        template<typename T, TEMPLATE_ENABLE(is_base<Engine, T>)>
        T* getEngine() const { return dynamic_cast<T*>(getEngine()); }

    private:

        Engine* m_Engine = nullptr;
    };
}
