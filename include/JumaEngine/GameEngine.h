// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "GameEngineBase.h"

namespace JumaEngine
{
    class GameEngine final : public GameEngineBase
    {
    public:
        GameEngine() = default;
        virtual ~GameEngine() override = default;

    private:

        bool createRenderEngine(JumaRE::RenderAPI api);
    };
}
