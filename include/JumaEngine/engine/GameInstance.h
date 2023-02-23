// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "EngineObject.h"
#include "IEngineObjectOwner.h"

#include <JumaRE/RenderTarget.h>
#include <JumaRE/input/InputButtons.h>

namespace JumaEngine
{
    class Widget;
    class WidgetsCreator;

    class GameInstance : public EngineObject, public IEngineObjectOwner
    {
        JUMAENGINE_CLASS(GameInstance, EngineObject)

        friend Engine;

    public:
        GameInstance() = default;
        virtual ~GameInstance() override = default;

        WidgetsCreator* getWidgetsCreator() const { return m_GameWidgetsCreator; }

    protected:

        virtual bool init();
        virtual bool initRenderData();
        virtual bool onSetupGameRenderTarget();

        virtual void onInitialized() override;
        virtual void onLogicStarted() override;
        virtual void onUpdate(float deltaTime) override;
        virtual void onPreRender() override;
        virtual void onLogicStopping() override;
        virtual void onDestroying() override;

        virtual void clear();
        virtual void clearRenderData();

        JumaRE::RenderTarget* getGameRenderTarget() const { return m_GameRenderTarget; }

        virtual void onInputButton(JumaRE::InputDevice device, JumaRE::InputButton button, JumaRE::InputButtonAction action) {}
        virtual void onInputAxis(JumaRE::InputDevice device, JumaRE::InputAxis axis, float value) {}
        virtual void onInputAxis2D(JumaRE::InputDevice device, JumaRE::InputAxis axis, const math::vector2& value) {}

    private:

        JumaRE::RenderTarget* m_GameRenderTarget = nullptr;

        WidgetsCreator* m_GameWidgetsCreator = nullptr;


        bool setupRenderTarget(JumaRE::RenderTarget* renderTarget);
    };
}
