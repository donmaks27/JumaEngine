// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"
#include "render/RenderedObject.h"

namespace JumaEngine
{
    class RenderTargetBase : public EngineContextObjectOld, public IRenderedObject
    {
        JUMAENGINE_CLASS_OLD(RenderTargetBase, EngineContextObjectOld)

    public:
        RenderTargetBase() = default;
        virtual ~RenderTargetBase() override = default;
    };
}
