// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/game/GameInstance.h"

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
        return true;
    }

    void GameInstance::onInitialized()
    {
        Super::onInitialized();
    }

    void GameInstance::onActivated()
    {
        Super::onActivated();

        m_GameWidgetsCreator = getEngine()->createObject<WidgetsCreator>();
        InitializeEngineObject(m_GameWidgetsCreator);
        ActivateEngineObject(m_GameWidgetsCreator);
    }

    void GameInstance::onUpdate(float deltaTime)
    {
        Super::onUpdate(deltaTime);

        UpdateEngineObject(m_GameWidgetsCreator, deltaTime);
    }

    void GameInstance::onPreRender()
    {
        Super::onPreRender();

        PreRenderEngineObject(m_GameWidgetsCreator);
    }

    void GameInstance::onDeactivate()
    {
        ClearEngineObject(m_GameWidgetsCreator);
        delete m_GameWidgetsCreator;
        m_GameWidgetsCreator = nullptr;

        Super::onDeactivate();
    }

    void GameInstance::onClear()
    {
        Super::onClear();
    }

    void GameInstance::clear()
    {
        clearRenderData();
    }
    void GameInstance::clearRenderData()
    {
    }
}
