// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include "render/IRenderInterface.h"

namespace JumaEngine
{
    class RenderTargetBase : public EngineContextObject, public IRenderInterface
    {
    public:
        RenderTargetBase() = default;
        virtual ~RenderTargetBase() override = default;
    };
}
