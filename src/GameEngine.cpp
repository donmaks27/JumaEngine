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
        const JumaRE::RenderEngine* renderEngine = getRenderEngine();
        JumaRE::WindowController* windowController = renderEngine->getWindowController();
        m_InitialGameInstanceRenderTarger = renderEngine->getRenderTarget(windowController->findWindowData(windowController->getMainWindowID())->windowRenderTargetID);
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
    void GameEngine::update()
    {
        Super::update();

        // TODO: Calculate delta time
        UpdateLogicObject(getGameInstance(), 0.0f);
    }
    void GameEngine::postUpdate()
    {
        Super::postUpdate();

        PostUpdateLogicObject(getGameInstance());
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
