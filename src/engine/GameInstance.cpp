// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/engine/GameInstance.h"

#include "JumaEngine/engine/Engine.h"
#include "JumaEngine/widgets/WidgetsCreator.h"

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

    void GameInstance::onInitialized()
    {
        Super::onInitialized();
    }

    void GameInstance::onLogicStarted()
    {
        Super::onLogicStarted();

        m_GameWidgetsCreator = getEngine()->createObject<WidgetsCreator>();
        InitializeLogicObject(m_GameWidgetsCreator);
        StartLogicObject(m_GameWidgetsCreator);
    }

    void GameInstance::onUpdate(float deltaTime)
    {
        Super::onUpdate(deltaTime);

        UpdateLogicObject(m_GameWidgetsCreator, deltaTime);
    }

    void GameInstance::onPreRender()
    {
        Super::onPreRender();

        PreRenderLogicObject(m_GameWidgetsCreator);
    }

    void GameInstance::onLogicStopping()
    {
        DestroyLogicObject(m_GameWidgetsCreator);
        delete m_GameWidgetsCreator;
        m_GameWidgetsCreator = nullptr;

        Super::onLogicStopping();
    }

    void GameInstance::onDestroying()
    {
        Super::onDestroying();
    }

    void GameInstance::clear()
    {
        clearRenderData();
    }
    void GameInstance::clearRenderData()
    {
    }
}
