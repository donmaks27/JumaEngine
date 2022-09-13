// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"

namespace JumaEngine
{
    class EngineBase;

    class EngineContextObject
    {
        friend EngineBase;

    public:
        EngineContextObject() = default;
        virtual ~EngineContextObject() = default;

        EngineBase* getEngine() const { return m_Engine; }
        template<typename T, TEMPLATE_ENABLE(is_base<EngineBase, T>)>
        T* getEngine() const { return dynamic_cast<T*>(m_Engine); }

    private:

        EngineBase* m_Engine = nullptr;
    };
}
