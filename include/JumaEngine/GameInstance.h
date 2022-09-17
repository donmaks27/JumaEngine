// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "EngineContextObject.h"

#include <JumaRE/RenderTarget.h>
#include <JumaRE/input/InputButtons.h>

namespace JumaEngine
{
    class GameInstance : public EngineContextObject
    {
        using Super = EngineContextObject;

        friend Engine;

    public:
        GameInstance() = default;
        virtual ~GameInstance() override;
        
        bool init(JumaRE::RenderTarget* renderTarget);
        void clear();

        virtual void update();

    protected:

        virtual bool initInternal();
        virtual void clearInternal();

        JumaRE::RenderTarget* getGameRenderTarget() const { return m_GameRenderTarget; }

    private:

        JumaRE::RenderTarget* m_GameRenderTarget = nullptr;

        math::uvector2 m_CursorPosition = { 0, 0 };


        void clearData_GameInstance();

        void onInputButton(JumaRE::InputDevice device, JumaRE::InputButton button, JumaRE::InputButtonAction action);
        void onInputAxis(JumaRE::InputDevice device, JumaRE::InputAxis axis, float value);
        void onInputAxis2D(JumaRE::InputDevice device, JumaRE::InputAxis axis, const math::vector2& value);
    };
}
