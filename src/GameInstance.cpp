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

    bool GameInstance::initLogic()
    {
        return true;
    }
    void GameInstance::startLogic()
    {
    }
    bool GameInstance::update(float deltaTime)
    {
        return true;
    }
    void GameInstance::stopLogic()
    {
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
        if (axis == JumaRE::InputAxis::Mouse2D)
        {
            if (m_GameRenderTarget != nullptr)
            {
                const math::uvector2 renderTargetSize = m_GameRenderTarget->getSize();
                const math::ivector2 newPosition = {
                    static_cast<int32>(m_CursorPosition.x) + math::round(value.x),
                    static_cast<int32>(m_CursorPosition.y) + math::round(value.y)
                };
                m_CursorPosition.x = math::clamp(newPosition.x, 0, math::max(renderTargetSize.x - 1, 0));
                m_CursorPosition.y = math::clamp(newPosition.y, 0, math::max(renderTargetSize.y - 1, 0));
            }
        }
    }
}
