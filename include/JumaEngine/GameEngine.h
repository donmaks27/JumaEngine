// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "Engine.h"

namespace JumaEngine
{
    class GameEngine final : public Engine
    {
        using Super = Engine;

    public:
        GameEngine() = default;
        virtual ~GameEngine() override;

        virtual bool update() override;

    protected:

        virtual bool initInternal() override;
        virtual bool initGameInstance() override;
        virtual void clearInternal() override;

    private:


        void clearData_GameEngine();
    };
}
