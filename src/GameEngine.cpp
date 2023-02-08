// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/GameEngine.h"

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
        m_InitialGameInstanceRenderTarger = getWindowRenderTarget(windowController->getMainWindowID());
        if (!Super::initEngineLoop())
        {
            return false;
        }

        windowController->onWindowInput.bind(this, &GameEngine::onWindowInput);
        InitializeLogicObject(getGameInstance());
        return true;
    }
    void GameEngine::onEngineLoopStarted()
    {
        Super::onEngineLoopStarted();

        StartLogicObject(getGameInstance());
    }
    void GameEngine::update(const float deltaTime)
    {
        Super::update(deltaTime);
        
        UpdateLogicObject(getGameInstance(), deltaTime);
    }
    void GameEngine::postUpdate()
    {
        Super::postUpdate();

        PreRenderLogicObject(getGameInstance());
    }
    void GameEngine::onEngineLoopStopped()
    {
        getRenderEngine()->getWindowController()->onWindowInput.unbind(this, &GameEngine::onWindowInput);

        DestroyLogicObject(getGameInstance());

        Super::onEngineLoopStopped();
    }

    void GameEngine::onWindowInput(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
        const JumaRE::InputActionData& input)
    {
        passInputToGameInstance(input);
    }
}

#endif
