// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"

#ifdef JUMAENGINE_ENABLED_GAMEENGINE

#include "Engine.h"
#include "ILogicObjectOwner.h"

namespace JumaEngine
{
    class GameEngine final : public Engine, public ILogicObjectOwner
    {
        using Super = Engine;

    public:
        GameEngine() = default;
        virtual ~GameEngine() override = default;

    protected:

        virtual bool initRenderEngine() override;

        virtual bool initEngineLoop() override;
        virtual void onEngineLoopStarted() override;
        virtual void update() override;
        virtual void postUpdate() override;
        virtual void onEngineLoopStopped() override;

    private:

        void onWindowInput(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, const JumaRE::InputActionData& input);
    };
}

#endif
