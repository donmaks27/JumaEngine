// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/widgets/WidgetsCreator.h"

#include "JumaEngine/engine/Engine.h"
#include "JumaEngine/widgets/Widget.h"
#include "JumaEngine/widgets/WidgetContext.h"

namespace JumaEngine
{
    void WidgetsCreator::onInitialized()
    {
        Super::onInitialized();
    }

    void WidgetsCreator::onActivated()
    {
        Super::onActivated();

        for (const auto& widgetContext : m_WidgetContexts)
        {
            Widget* rootWidget = widgetContext.value.getRootWidget();
            if (rootWidget != nullptr)
            {
                ActivateEngineObject(rootWidget);
            }
        }
        for (const auto& widget : m_Widgets)
        {
            ActivateEngineObject(widget);
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
                UpdateEngineObject(rootWidget, deltaTime);
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
                rootWidget->setWidgetBounds({ { 0.0f, 0.0f }, { 1.0f, 1.0f } }, WidgetAlignmentH::Fill, WidgetAlignmentV::Fill);
                rootWidget->recalculateWidetSize();

                PreRenderEngineObject(rootWidget);
            }
        }
    }

    void WidgetsCreator::onDeactivate()
    {
        for (const auto& widget : m_Widgets)
        {
            DeactivateEngineObject(widget);
        }

        Super::onDeactivate();
    }

    void WidgetsCreator::onClear()
    {
        for (const auto& widget : m_Widgets)
        {
            ClearEngineObject(widget);
        }
        for (const auto& widget : m_Widgets)
        {
            delete widget;
        }
        m_Widgets.clear();
        m_WidgetContexts.clear();

        Super::onClear();
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
        InitializeEngineObject(widget);
        return widget;
    }
    bool WidgetsCreator::destroyWidget(Widget* widget, const bool destroyChildWidgets)
    {
        if ((widget == nullptr) || (widget->getWidgetsCreator() != this))
        {
	        return false;
        }

        jarray<Widget*> widgetsToDestroy = { widget };
        while (!widgetsToDestroy.isEmpty())
        {
	        Widget* lastWidget = widgetsToDestroy.getLast();
            widgetsToDestroy.removeLast();
            if (lastWidget != nullptr)
            {
	            widgetsToDestroy.append(lastWidget->getChildWidgets());

                ClearEngineObject(lastWidget);
	            m_Widgets.remove(lastWidget);
	            delete lastWidget;
            }
        }
        return true;
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

            if (isActive())
            {
                ActivateEngineObject(widget);
            }
        }
    }
}
