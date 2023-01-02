// Copyright © 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "EngineContextObject.h"

#include <JumaRE/RenderTarget.h>
#include <JumaRE/input/InputButtons.h>

namespace JumaEngine
{
    class GameInstance : public EngineContextObject
    {
        JUMAENGINE_CLASS(GameInstance, EngineContextObject)

        friend Engine;
#ifdef JUMAENGINE_ENABLED_GAMEENGINE
        friend class GameEngine;
#endif

    public:
        GameInstance() = default;
        virtual ~GameInstance() override = default;

    protected:

        virtual bool init();
        virtual bool initRenderData();
        virtual bool onSetupGameRenderTarget();

        virtual bool initLogic();
        virtual void startLogic();
        virtual bool update(float deltaTime);
        virtual void stopLogic();

        virtual void clear();
        virtual void clearRenderData();

        JumaRE::RenderTarget* getGameRenderTarget() const { return m_GameRenderTarget; }
        const math::uvector2& getCursorPosition() const { return m_CursorPosition; }

        virtual void onInputButton(JumaRE::InputDevice device, JumaRE::InputButton button, JumaRE::InputButtonAction action);
        virtual void onInputAxis(JumaRE::InputDevice device, JumaRE::InputAxis axis, float value);
        virtual void onInputAxis2D(JumaRE::InputDevice device, JumaRE::InputAxis axis, const math::vector2& value);

    private:

        JumaRE::RenderTarget* m_GameRenderTarget = nullptr;

        math::uvector2 m_CursorPosition = { 0, 0 };


        bool setupRenderTarget(JumaRE::RenderTarget* renderTarget);
    };
}
