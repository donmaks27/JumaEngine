// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/WidgetsCreator.h"

#include "JumaEngine/Engine.h"
#include "JumaEngine/subsystems/ui/Widget.h"
#include "JumaEngine/subsystems/ui/WidgetContext.h"

namespace JumaEngine
{
    void WidgetsCreator::onInitialized()
    {
        Super::onInitialized();
    }

    void WidgetsCreator::onLogicStarted()
    {
        Super::onLogicStarted();

        for (const auto& widgetContext : m_WidgetContexts)
        {
            Widget* rootWidget = widgetContext.value.getRootWidget();
            if (rootWidget != nullptr)
            {
                StartLogicObject(rootWidget);
            }
        }
        for (const auto& widget : m_Widgets)
        {
            StartLogicObject(widget);
        }
    }

    void WidgetsCreator::onUpdate(const float deltaTime)
    {
        Super::onUpdate(deltaTime);

        for (const auto& widgetContext : m_WidgetContexts)
        {
            Widget* rootWidget = widgetContext.value.getRootWidget();
            if (rootWidget != nullptr)
            {
                UpdateLogicObject(rootWidget, deltaTime);
            }
        }
    }

    void WidgetsCreator::onPreRender()
    {
        Super::onPreRender();

        for (const auto& widgetContext : m_WidgetContexts)
        {
            Widget* rootWidget = widgetContext.value.getRootWidget();
            if (rootWidget != nullptr)
            {
                PreRenderLogicObject(rootWidget);
            }
        }
    }

    void WidgetsCreator::onLogicStopping()
    {
        for (const auto& widget : m_Widgets)
        {
            StopLogicObject(widget);
        }

        Super::onLogicStopping();
    }

    void WidgetsCreator::onDestroying()
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
        m_WidgetContexts.clear();

        Super::onDestroying();
    }

    WidgetContext* WidgetsCreator::createWidgetContext(JumaRE::RenderTarget* renderTarget)
    {
        if (!isInitialized())
        {
            JUTILS_LOG(error, JSTR("Widgets creator not initialized"));
            return nullptr;
        }

        if (renderTarget == nullptr)
        {
            JUTILS_LOG(warning, JSTR("Render target is null"));
            return nullptr;
        }

        if (m_WidgetContexts.find(renderTarget) != nullptr)
        {
            JUTILS_LOG(warning, JSTR("Widget context already created"));
            return nullptr;
        }

        WidgetContext& widgetContext = m_WidgetContexts.add(renderTarget);
        widgetContext.m_ParentWidgetsCreator = this;
        widgetContext.m_RenderTarget = renderTarget;
        return &widgetContext;
    }
    void WidgetsCreator::destroyWidgetContext(WidgetContext* widgetContext)
    {
        JumaRE::RenderTarget* renderTarget = widgetContext != nullptr ? widgetContext->getRenderTarget() : nullptr;
        if ((renderTarget != nullptr) && m_WidgetContexts.contains(renderTarget))
        {
            Widget* widget = widgetContext->getRootWidget();
            if (widget != nullptr)
            {
                widget->setParentWidget(nullptr);
            }
            m_WidgetContexts.remove(renderTarget);
        }
    }

    Widget* WidgetsCreator::createWidget(const EngineSubclass<Widget>& widgetClass)
    {
        if (!isInitialized())
        {
            JUTILS_LOG(error, JSTR("Widgets creator not initialized"));
            return nullptr;
        }

        Widget* widget = getEngine()->createObject<Widget>(widgetClass);
        if (widget == nullptr)
        {
            return nullptr;
        }

        m_Widgets.add(widget);
        widget->m_ParentWidgetsCreator = this;
        InitializeLogicObject(widget);
        return widget;
    }
    void WidgetsCreator::destroyWidget(Widget* widget)
    {
        if (widget != nullptr)
        {
            DestroyLogicObject(widget);
            m_Widgets.remove(widget);
            delete widget;
        }
    }

    void WidgetsCreator::setRootWidget(WidgetContext* widgetContext, Widget* widget)
    {
        if ((widgetContext == nullptr) || (widgetContext->getWidgetsCreator() != this))
        {
            return;
        }
        Widget* oldRootWidget = widgetContext->getRootWidget();
        if (oldRootWidget == widget)
        {
            return;
        }

        if (oldRootWidget != nullptr)
        {
            oldRootWidget->setWidgetContext(nullptr);
        }
        widgetContext->m_RootWidget = widget;
        if (widget != nullptr)
        {
            widget->setParentWidget(nullptr);
            widget->setWidgetContext(widgetContext);

            if (isLogicActive())
            {
                StartLogicObject(widget);
            }
        }
    }
}
