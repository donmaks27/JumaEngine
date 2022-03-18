// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class RenderTarget : public EngineContextObject
    {
        JUMAENGINE_ABSTRACT_CLASS(RenderTarget, EngineContextObject)

    public:
        RenderTarget() = default;
        virtual ~RenderTarget() override = default;
    };
}
