// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/GameEngine.h"

#ifdef JUMAENGINE_MODE_GAME

#include "JumaEngine/render/RenderEngineSubsystem.h"

namespace JumaEngine
{
    bool GameEngine::initRenderEngine()
    {
        m_InitialRenderAPI = JumaRE::RenderAPI::DirectX11;
        return Super::initRenderEngine();
    }

    JumaRE::RenderTarget* GameEngine::getGameInstanceRenderTarget() const
    {
        const JumaRE::RenderEngine* renderEngine = getRenderEngine();
        const JumaRE::WindowController* windowController = renderEngine != nullptr ? renderEngine->getWindowController() : nullptr;
        const JumaRE::window_id windowID = windowController != nullptr ? windowController->getMainWindowID() : JumaRE::window_id_INVALID;
        if (windowID == JumaRE::window_id_INVALID)
        {
	        return nullptr;
        }

        const RenderEngineSubsystem* renderSubsystem = getSubsystem<RenderEngineSubsystem>();
        return renderSubsystem != nullptr ? renderSubsystem->getWindowRenderTarget(windowID) : nullptr;
    }

    bool GameEngine::onEngineLoopStarting()
    {
        if (!Super::onEngineLoopStarting())
        {
            return false;
        }

        JumaRE::WindowController* windowController = getRenderEngine()->getWindowController();
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
    void GameEngine::onEngineLoopStopping()
    {
        getRenderEngine()->getWindowController()->onWindowInput.unbind(this, &GameEngine::onWindowInput);

        DestroyLogicObject(getGameInstance());

        Super::onEngineLoopStopping();
    }

    void GameEngine::onWindowInput(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
        const JumaRE::InputActionData& input)
    {
        passInputToGameInstance(input);
    }
}

#endif
