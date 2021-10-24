// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderTargetBase.h"

namespace JumaEngine
{
    class RenderTargetCompositeBase : public RenderTargetBase
    {
        JUMAENGINE_CLASS_OLD(RenderTargetCompositeBase, RenderTargetBase)

    public:
        RenderTargetCompositeBase() = default;
        virtual ~RenderTargetCompositeBase() override = default;
    };
}
