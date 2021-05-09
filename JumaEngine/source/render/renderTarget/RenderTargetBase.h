// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"
#include "render/IRenderInterface.h"

namespace JumaEngine
{
    class RenderTargetBase : public EngineContextObject, public IRenderInterface
    {
        JUMAENGINE_CLASS(RenderTargetBase, EngineContextObject)

    public:
        RenderTargetBase() = default;
        virtual ~RenderTargetBase() override = default;
    };
}
