// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "EngineBase.h"

namespace JumaEngine
{
    class GameEngineBase : public EngineBase
    {
        using Super = EngineBase;

    public:
        GameEngineBase() = default;
        virtual ~GameEngineBase() override = default;
    };
}
