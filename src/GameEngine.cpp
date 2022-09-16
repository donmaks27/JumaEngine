// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../include/JumaEngine/GameEngine.h"

#include <JumaRE/RenderPipeline.h>
#include <JumaRE/RenderTarget.h>

#include "../include/JumaEngine/GameInstance.h"

namespace JumaEngine
{
    GameEngine::~GameEngine()
    {
        clearData_GameEngine();
    }

    bool GameEngine::initInternal()
    {
        if (!Super::initInternal())
        {
            return false;
        }
        return createRenderEngine(JumaRE::RenderAPI::Vulkan, { JSTR("JumaEngine"), { 800, 600 } });
    }

    bool GameEngine::initGameInstance()
    {
        const JumaRE::RenderEngine* renderEngine = getRenderEngine();
        const JumaRE::WindowController* windowController = renderEngine != nullptr ? renderEngine->getWindowController() : nullptr;
        const JumaRE::window_id windowID = windowController != nullptr ? windowController->getMainWindowID() : JumaRE::window_id_INVALID;
        const JumaRE::WindowData* windowData = windowController != nullptr ? windowController->findWindowData(windowID) : nullptr;
        JumaRE::RenderTarget* renderTarget = windowData != nullptr ? windowData->windowRenderTarget : nullptr;
        return getGameInstance()->init(renderTarget);
    }

    void GameEngine::clearInternal()
    {
        clearData_GameEngine();
        Super::clearInternal();
    }
    void GameEngine::clearData_GameEngine()
    {
        const JumaRE::RenderEngine* renderEngine = getRenderEngine();
        JumaRE::RenderPipeline* pipeline = renderEngine != nullptr ? renderEngine->getRenderPipeline() : nullptr;
        if (pipeline != nullptr)
        {
            pipeline->waitForRenderFinished();
        }

        destroyGameInstance();
    }

    bool GameEngine::update()
    {
        const JumaRE::RenderEngine* renderEngine = getRenderEngine();
        if (renderEngine == nullptr)
        {
            return false;
        }

        JumaRE::WindowController* windowController = renderEngine->getWindowController();
        if (windowController->isMainWindowClosed())
        {
            return false;
        }

        getGameInstance()->update();

        JumaRE::RenderPipeline* pipeline = renderEngine->getRenderPipeline();
        if (!pipeline->render())
        {
            return false;
        }

        windowController->updateWindows();
        return true;
    }
}
