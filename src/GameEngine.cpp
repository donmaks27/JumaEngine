// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../include/JumaEngine/GameEngine.h"

#include <JumaRE/RenderPipeline.h>

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
        return createRenderEngine(JumaRE::RenderAPI::Vulkan);
    }
    bool GameEngine::createRenderEngine(const JumaRE::RenderAPI api)
    {
        JumaRE::RenderEngine* renderEngine = JumaRE::CreateRenderEngine(api);
        if (renderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create render engine ({})"), api);
            return false;
        }
        if (!renderEngine->init({ JSTR("JumaEngine"), { 800, 600 }, JumaRE::TextureSamples::X1 }))
        {
            JUTILS_LOG(error, JSTR("Failed to initialize render engine ({})"), api);
            return false;
        }

        JUTILS_LOG(correct, JSTR("Render engine ({}) initialized"), api);
        m_RenderEngine = renderEngine;
        return true;
    }

    bool GameEngine::update()
    {
        JumaRE::WindowController* windowController = m_RenderEngine->getWindowController();
        if (windowController->shouldCloseWindow(windowController->getMainWindowID()))
        {
            return false;
        }

        if (!m_RenderEngine->getRenderPipeline()->render())
        {
            return false;
        }

        windowController->updateWindows();
        return true;
    }

    void GameEngine::destroyInternal()
    {
        clearData_GameEngine();
        Super::destroyInternal();
    }
    void GameEngine::clearData_GameEngine()
    {
        if (m_RenderEngine != nullptr)
        {
            JumaRE::RenderPipeline* pipeline = m_RenderEngine->getRenderPipeline();
            if (pipeline != nullptr)
            {
                pipeline->waitForRenderFinished();
            }

            delete m_RenderEngine;
            m_RenderEngine = nullptr;
        }
    }
}
