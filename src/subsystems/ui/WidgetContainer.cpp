// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/WidgetContainer.h"

#include "JumaEngine/Engine.h"
#include "JumaEngine/subsystems/ui/Widget.h"

namespace JumaEngine
{
    void WidgetContainer::onInitialized()
    {
        Super::onInitialized();

        if (m_RootWidget != nullptr)
        {
            m_RootWidget->setWidgetContainer(this);
            InitializeLogicObject(m_RootWidget);
        }
    }

    void WidgetContainer::onLogicStarted()
    {
        Super::onLogicStarted();

        if (m_RootWidget != nullptr)
        {
            StartLogicObject(m_RootWidget);
        }
    }

    void WidgetContainer::onUpdate(const float deltaTime)
    {
        Super::onUpdate(deltaTime);

        if (m_RootWidget != nullptr)
        {
            UpdateLogicObject(m_RootWidget, deltaTime);
        }
    }

    void WidgetContainer::onPostUpdate()
    {
        Super::onPostUpdate();

        if (m_RootWidget != nullptr)
        {
            PostUpdateLogicObject(m_RootWidget);
        }
    }

    void WidgetContainer::onLogicStopping()
    {
        if (m_RootWidget != nullptr)
        {
            StopLogicObject(m_RootWidget);
        }

        Super::onLogicStopping();
    }

    void WidgetContainer::onDestroying()
    {
        if (m_RootWidget != nullptr)
        {
            DestroyLogicObject(m_RootWidget);
            delete m_RootWidget;
            m_RootWidget = nullptr;
        }

        m_RenderTarget = nullptr;

        Super::onDestroying();
    }

    Widget* WidgetContainer::setRootWidget(const EngineSubclass<Widget>& widgetClass)
    {
        if (isDestroyed())
        {
            return nullptr;
        }

        if (m_RootWidget != nullptr)
        {
            DestroyLogicObject(m_RootWidget);
            delete m_RootWidget;
        }

        m_RootWidget = getEngine()->createObject(widgetClass);
        if (m_RootWidget != nullptr)
        {
            m_RootWidget->setWidgetContainer(this);
            if (isInitialized())
            {
                InitializeLogicObject(m_RootWidget);
                if (isLogicActive())
                {
                    StartLogicObject(m_RootWidget);
                }
            }
        }

        return getRootWidget();
    }
}
