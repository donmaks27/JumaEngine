// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/OverlayWidget.h"

#include "JumaEngine/Engine.h"

namespace JumaEngine
{
    void OverlayWidget::onInitialized()
    {
        Super::onInitialized();

        for (const auto& widget : m_Widgets)
        {
            InitializeLogicObject(widget);
        }
    }

    void OverlayWidget::onLogicStarted()
    {
        Super::onLogicStarted();

        for (const auto& widget : m_Widgets)
        {
            StartLogicObject(widget);
        }
    }

    void OverlayWidget::onUpdate(const float deltaTime)
    {
        Super::onUpdate(deltaTime);

        for (const auto& widget : m_Widgets)
        {
            UpdateLogicObject(widget, deltaTime);
        }
    }

    void OverlayWidget::onPostUpdate()
    {
        Super::onPostUpdate();

        for (const auto& widget : m_Widgets)
        {
            PostUpdateLogicObject(widget);
        }
    }

    void OverlayWidget::onLogicStopping()
    {
        for (const auto& widget : m_Widgets)
        {
            StopLogicObject(widget);
        }

        Super::onLogicStopping();
    }

    void OverlayWidget::onDestroying()
    {
        for (const auto& widget : m_Widgets)
        {
            DestroyLogicObject(widget);
        }
        for (const auto& widget : m_Widgets)
        {
            delete widget;
        }
        m_Widgets.clear();

        Super::onDestroying();
    }

    Widget* OverlayWidget::addWidget(const EngineSubclass<Widget>& widgetClass)
    {
        if (isDestroyed())
        {
            return nullptr;
        }

        Widget* widget = m_Widgets.add(getEngine()->createObject(widgetClass));
        widget->setParentWidget(this);
        if (isInitialized())
        {
            InitializeLogicObject(widget);
            if (isLogicActive())
            {
                StartLogicObject(widget);
            }
        }
        return widget;
    }
}
