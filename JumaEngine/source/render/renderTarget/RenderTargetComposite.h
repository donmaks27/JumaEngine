// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderTargetBase.h"

namespace JumaEngine
{
    class RenderTargetComposite : public RenderTargetBase
    {
    public:
        RenderTargetComposite() = default;
        virtual ~RenderTargetComposite() override = default;
    };
}
