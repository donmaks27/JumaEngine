// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/GameInstance.h"

#include "JumaEngine/Engine.h"
#include "JumaEngine/subsystems/ui/WidgetContainer.h"

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
    }

    void GameInstance::onUpdate(float deltaTime)
    {
        Super::onUpdate(deltaTime);

        for (const auto& widgetContainer : m_WidgetContainers)
        {
            UpdateLogicObject(widgetContainer, deltaTime);
        }
    }

    void GameInstance::onPostUpdate()
    {
        Super::onPostUpdate();

        for (const auto& widgetContainer : m_WidgetContainers)
        {
            PostUpdateLogicObject(widgetContainer);
        }
    }

    void GameInstance::onLogicStopping()
    {
        for (const auto& widgetContainer : m_WidgetContainers)
        {
            DestroyLogicObject(widgetContainer);
        }
        for (const auto& widgetContainer : m_WidgetContainers)
        {
            delete widgetContainer;
        }
        m_WidgetContainers.clear();

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

    WidgetContainer* GameInstance::createWidget(JumaRE::RenderTarget* renderTarget)
    {
        if (!isLogicActive())
        {
            return nullptr;
        }

        WidgetContainer* widgetContainer = getEngine()->createObject<WidgetContainer>();
        widgetContainer->setRenderTarget(renderTarget);
        InitializeLogicObject(widgetContainer);
        StartLogicObject(widgetContainer);
        return m_WidgetContainers.add(widgetContainer);
    }
    void GameInstance::destroyWidget(WidgetContainer* widgetContainer)
    {
        const int32 index = m_WidgetContainers.indexOf(widgetContainer);
        if (m_WidgetContainers.isValidIndex(index))
        {
            DestroyLogicObject(widgetContainer);
            m_WidgetContainers.removeAt(index);
            delete widgetContainer;
        }
    }
}
