// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../include/JumaEngine/GameEngine.h"

#ifdef JUMAENGINE_ENABLED_GAMEENGINE

namespace JumaEngine
{
    bool GameEngine::initRenderEngine()
    {
        m_InitialRenderAPI = JumaRE::RenderAPI::Vulkan;
        return Super::initRenderEngine();
    }

    bool GameEngine::initEngineLoop()
    {
        JumaRE::WindowController* windowController = getRenderEngine()->getWindowController();
        m_InitialGameInstanceRenderTarger = windowController->findWindowData(windowController->getMainWindowID())->windowRenderTarget;
        if (!Super::initEngineLoop())
        {
            return false;
        }

        windowController->OnInputButton.bind(this, &GameEngine::onInputButton);
        windowController->OnInputAxis.bind(this, &GameEngine::onInputAxis);
        windowController->OnInputAxis2D.bind(this, &GameEngine::onInputAxis2D);

        return getGameInstance()->initLogic();
    }
    void GameEngine::startEngineLoop()
    {
        Super::startEngineLoop();
        getGameInstance()->startLogic();
    }
    bool GameEngine::update()
    {
        if (!Super::update())
        {
            return false;
        }
        // TODO: Calculate delta time
        getGameInstance()->update(0.0f);
        return true;
    }
    void GameEngine::stopEngineLoop()
    {
        getGameInstance()->stopLogic();
        Super::stopEngineLoop();
    }

    void GameEngine::onInputButton(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData,
        const JumaRE::InputDevice device, const JumaRE::InputButton button, const JumaRE::InputButtonAction action)
    {
        getGameInstance()->onInputButton(device, button, action);
    }
    void GameEngine::onInputAxis(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData,
        const JumaRE::InputDevice device, const JumaRE::InputAxis axis, const float value)
    {
        getGameInstance()->onInputAxis(device, axis, value);
    }
    void GameEngine::onInputAxis2D(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData,
        const JumaRE::InputDevice device, const JumaRE::InputAxis axis, const math::vector2& value)
    {
        getGameInstance()->onInputAxis2D(device, axis, value);
    }
}

#endif
