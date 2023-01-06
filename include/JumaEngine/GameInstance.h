// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "LogicObject.h"
#include "ILogicObjectOwner.h"

#include <JumaRE/RenderTarget.h>
#include <JumaRE/input/InputButtons.h>

namespace JumaEngine
{
    class GameInstance : public LogicObject, public ILogicObjectOwner
    {
        JUMAENGINE_CLASS(GameInstance, LogicObject)

        friend Engine;

    public:
        GameInstance() = default;
        virtual ~GameInstance() override = default;

    protected:

        virtual bool init();
        virtual bool initRenderData();
        virtual bool onSetupGameRenderTarget();

        virtual void clear();
        virtual void clearRenderData();

        JumaRE::RenderTarget* getGameRenderTarget() const { return m_GameRenderTarget; }

        virtual void onInputButton(JumaRE::InputDevice device, JumaRE::InputButton button, JumaRE::InputButtonAction action);
        virtual void onInputAxis(JumaRE::InputDevice device, JumaRE::InputAxis axis, float value);
        virtual void onInputAxis2D(JumaRE::InputDevice device, JumaRE::InputAxis axis, const math::vector2& value);

    private:

        JumaRE::RenderTarget* m_GameRenderTarget = nullptr;


        bool setupRenderTarget(JumaRE::RenderTarget* renderTarget);
    };
}
