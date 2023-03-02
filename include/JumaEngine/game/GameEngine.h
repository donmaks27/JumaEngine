// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

#ifdef JUMAENGINE_MODE_GAME

#include "../engine/Engine.h"

namespace JumaEngine
{
    class GameEngine final : public Engine
    {
        using Super = Engine;

    public:
        GameEngine() = default;
        virtual ~GameEngine() override = default;

    protected:
        
        virtual JumaRE::RenderAPI getDesiredRenderAPI() const override;
        virtual jstring getWindowsTitle() const override;
        virtual EngineObjectPtr<RenderTarget> getGameInstanceRenderTarget() const override;

        virtual bool onEngineLoopStarting() override;
        virtual void onEngineLoopStarted() override;
        virtual void update(float deltaTime) override;
        virtual void preRender() override;
        virtual void onEngineLoopStopped() override;

    private:

        void onWindowInput(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, const JumaRE::InputActionData& input);
    };
}

#endif
