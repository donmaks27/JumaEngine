// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/game/GameInstance.h"

#include "JumaEngine/engine/Engine.h"
#include "JumaEngine/widgets/WidgetsCreator.h"

namespace JumaEngine
{
    bool GameInstance::setupRenderTarget(EngineObjectPtr<RenderTarget> renderTarget)
    {
        m_GameRenderTarget = std::move(renderTarget);
        return true;
    }

    void GameInstance::onInitialized()
    {
        Super::onInitialized();

        m_GameWidgetsCreator = getEngine()->createObject<WidgetsCreator>();
        InitializeEngineObject(m_GameWidgetsCreator.get());
    }

    void GameInstance::onActivated()
    {
        Super::onActivated();

        ActivateEngineObject(m_GameWidgetsCreator.get());
    }

    void GameInstance::onUpdate(const float deltaTime)
    {
        Super::onUpdate(deltaTime);

        UpdateEngineObject(m_GameWidgetsCreator.get(), deltaTime);
    }

    void GameInstance::onPreRender()
    {
        Super::onPreRender();

        PreRenderEngineObject(m_GameWidgetsCreator.get());
    }

    void GameInstance::onDeactivate()
    {
        DeactivateEngineObject(m_GameWidgetsCreator.get());

        Super::onDeactivate();
    }

    void GameInstance::onClear()
    {
        ClearEngineObject(m_GameWidgetsCreator.get());
        m_GameWidgetsCreator = nullptr;

        Super::onClear();
    }
}
