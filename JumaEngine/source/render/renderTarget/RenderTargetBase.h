// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"
#include "render/RenderedObject.h"

namespace JumaEngine
{
    class RenderTargetBase : public EngineContextObject, public IRenderedObject
    {
        JUMAENGINE_CLASS(RenderTargetBase, EngineContextObject)

    public:
        RenderTargetBase() = default;
        virtual ~RenderTargetBase() override = default;
    };
}
