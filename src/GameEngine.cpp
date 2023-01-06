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

        return CallInitLogicObject(getGameInstance());
    }
    void GameEngine::onEngineLoopStarted()
    {
        Super::onEngineLoopStarted();

        CallOnStartLogic(getGameInstance());
    }
    void GameEngine::update()
    {
        Super::update();

        // TODO: Calculate delta time
        CallUpdateLogicObject(getGameInstance(), 0.0f);
    }
    void GameEngine::postUpdate()
    {
        Super::postUpdate();

        CallPostUpdateLogicObject(getGameInstance());
    }
    void GameEngine::onEngineLoopStopped()
    {
        CallClearLogicObject(getGameInstance());

        getRenderEngine()->getWindowController()->onWindowInput.unbind(this, &GameEngine::onWindowInput);

        Super::onEngineLoopStopped();
    }

    void GameEngine::onWindowInput(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
        const JumaRE::InputActionData& input)
    {
        passInputToGameInstance(input);
    }
}

#endif
