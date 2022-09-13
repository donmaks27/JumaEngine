// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"

#include <JumaRE/RenderEngine.h>

namespace JumaEngine
{
    class EngineBase
    {
    public:
        EngineBase() = default;
        virtual ~EngineBase() = default;

        JumaRE::RenderEngine* getRenderEngine() const { return m_RenderEngine; }

    protected:

        JumaRE::RenderEngine* m_RenderEngine = nullptr;
    };
}
