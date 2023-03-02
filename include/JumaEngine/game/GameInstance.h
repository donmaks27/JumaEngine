// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineObject.h"
#include "../engine/EngineObjectOwner.h"

#include <JumaRE/RenderTarget.h>
#include <JumaRE/input/InputButtons.h>

namespace JumaEngine
{
    class RenderTarget;
    class Widget;
    class WidgetsCreator;

    class GameInstance : public EngineObject, public EngineObjectOwner
    {
        JUMAENGINE_CLASS(GameInstance, EngineObject)

        friend Engine;

    public:
        GameInstance() = default;
        virtual ~GameInstance() override = default;

        WidgetsCreator* getWidgetsCreator() const { return m_GameWidgetsCreator.get(); }

    protected:

        virtual void onInitialized() override;
        virtual void onActivated() override;
        virtual void onUpdate(float deltaTime) override;
        virtual void onPreRender() override;
        virtual void onDeactivate() override;
        virtual void onClear() override;

        const EngineObjectPtr<RenderTarget>& getGameRenderTarget() const { return m_GameRenderTarget; }

        virtual void onInputButton(JumaRE::InputDevice device, JumaRE::InputButton button, JumaRE::InputButtonAction action) {}
        virtual void onInputAxis(JumaRE::InputDevice device, JumaRE::InputAxis axis, float value) {}
        virtual void onInputAxis2D(JumaRE::InputDevice device, JumaRE::InputAxis axis, const math::vector2& value) {}

    private:

        EngineObjectPtr<RenderTarget> m_GameRenderTarget = nullptr;

        EngineObjectPtr<WidgetsCreator> m_GameWidgetsCreator = nullptr;


        bool setupRenderTarget(EngineObjectPtr<RenderTarget> renderTarget);
    };
}
