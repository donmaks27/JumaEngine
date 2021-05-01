// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderTargerBase.h"

namespace JumaEngine
{
    class RenderTargetComposite : public RenderTargerBase
    {
    public:
        RenderTargetComposite() = default;
        virtual ~RenderTargetComposite() override = default;
    };
}
