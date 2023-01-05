// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"

#ifdef JUMAENGINE_ENABLED_GAMEENGINE

#include "Engine.h"

namespace JumaEngine
{
    class GameEngine final : public Engine
    {
        using Super = Engine;

    public:
        GameEngine() = default;
        virtual ~GameEngine() override = default;

    protected:

        virtual bool initRenderEngine() override;

        virtual bool initEngineLoop() override;
        virtual void startEngineLoop() override;
        virtual void update() override;
        virtual void postUpdate() override;
        virtual void stopEngineLoop() override;

    private:

        void onInputButton(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
            JumaRE::InputDevice device, JumaRE::InputButton button, JumaRE::InputButtonAction action);
        void onInputAxis(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
            JumaRE::InputDevice device, JumaRE::InputAxis axis, float value);
        void onInputAxis2D(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
            JumaRE::InputDevice device, JumaRE::InputAxis axis, const math::vector2& value);
    };
}

#endif
