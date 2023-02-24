﻿// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineObject.h"
#include "../engine/EngineObjectOwner.h"

#include <JumaRE/RenderTarget.h>
#include <JumaRE/input/InputButtons.h>

namespace JumaEngine
{
    class Widget;
    class WidgetsCreator;

    class GameInstance : public EngineObject, public EngineObjectOwner
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

        virtual void onInitialized() override;
        virtual void onActivated() override;
        virtual void onUpdate(float deltaTime) override;
        virtual void onPreRender() override;
        virtual void onDeactivate() override;
        virtual void onClear() override;

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
