// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "GameEngineBase.h"

namespace JumaEngine
{
    class GameEngine final : public GameEngineBase
    {
        using Super = GameEngineBase;

    public:
        GameEngine() = default;
        virtual ~GameEngine() override;

        virtual bool update() override;

    protected:

        virtual bool initInternal() override;
        virtual void destroyInternal() override;

    private:

        bool createRenderEngine(JumaRE::RenderAPI api);

        void clearData_GameEngine();
    };
}
