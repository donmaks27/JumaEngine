// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/GameEngine.h"

#ifdef JUMAENGINE_ENABLED_GAMEENGINE

#include <chrono>

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

        static std::chrono::time_point<std::chrono::steady_clock> prevTimePoint = std::chrono::steady_clock::now();
        const std::chrono::time_point<std::chrono::steady_clock> currentTimePoint = std::chrono::steady_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTimePoint - prevTimePoint);
        prevTimePoint = prevTimePoint + duration;
        
        const float deltaTime = static_cast<float>(duration.count()) / 1000.0f;

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
