// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/GameInstance.h"

namespace JumaEngine
{
    bool GameInstance::init()
    {
        return true;
    }
    bool GameInstance::initRenderData()
    {
        return true;
    }

    bool GameInstance::setupRenderTarget(JumaRE::RenderTarget* renderTarget)
    {
        m_GameRenderTarget = renderTarget;
        return onSetupGameRenderTarget();
    }
    bool GameInstance::onSetupGameRenderTarget()
    {
        return true;
    }

    void GameInstance::clear()
    {
        clearRenderData();
    }
    void GameInstance::clearRenderData()
    {
    }

    void GameInstance::onInputButton(const JumaRE::InputDevice device, const JumaRE::InputButton button, const JumaRE::InputButtonAction action)
    {
    }
    void GameInstance::onInputAxis(const JumaRE::InputDevice device, const JumaRE::InputAxis axis, const float value)
    {
    }
    void GameInstance::onInputAxis2D(const JumaRE::InputDevice device, const JumaRE::InputAxis axis, const math::vector2& value)
    {
    }
}
