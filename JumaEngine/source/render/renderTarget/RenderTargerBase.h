// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include "render/IRenderObject.h"

namespace JumaEngine
{
    class RenderTargerBase : public EngineContextObject, public IRenderObject
    {
    public:
        RenderTargerBase() = default;
        virtual ~RenderTargerBase() override = default;
    };
}
