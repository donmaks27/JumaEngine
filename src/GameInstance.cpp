// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../include/JumaEngine/GameInstance.h"

namespace JumaEngine
{
    GameInstance::~GameInstance()
    {
        clearData_GameInstance();
    }

    bool GameInstance::init(JumaRE::RenderTarget* renderTarget)
    {
        if (renderTarget == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invalid render target for game instance"));
            return false;
        }

        m_GameRenderTarget = renderTarget;
        return initInternal();
    }
    bool GameInstance::initInternal()
    {
        return true;
    }

    void GameInstance::clear()
    {
        clearInternal();
    }
    void GameInstance::clearInternal()
    {
        clearData_GameInstance();
    }
    void GameInstance::clearData_GameInstance()
    {
        m_GameRenderTarget = nullptr;
    }

    void GameInstance::update()
    {
    }

    void GameInstance::onInputButton(const JumaRE::InputDevice device, const JumaRE::InputButton button,
        const JumaRE::InputButtonAction action)
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

                JUTILS_LOG(info, JSTR("Cursor position: {}"), m_CursorPosition.toString());
            }
        }
    }
}
