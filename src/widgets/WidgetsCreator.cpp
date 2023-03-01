// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/widgets/WidgetsCreator.h"

#include "JumaEngine/engine/Engine.h"
#include "JumaEngine/widgets/Widget.h"
#include "JumaEngine/widgets/WidgetContext.h"

namespace JumaEngine
{
    void WidgetsCreator::onActivated()
    {
        Super::onActivated();

        for (const auto& widgetContext : m_WidgetContexts)
        {
            ActivateEngineObject(widgetContext.value);
        }
    }

    void WidgetsCreator::onUpdate(const float deltaTime)
    {
        Super::onUpdate(deltaTime);

        for (const auto& widgetContext : m_WidgetContexts)
        {
            UpdateEngineObject(widgetContext.value, deltaTime);
        }
    }

    void WidgetsCreator::onPreRender()
    {
        Super::onPreRender();

        for (const auto& widgetContext : m_WidgetContexts)
        {
            PreRenderEngineObject(widgetContext.value);
        }
    }

    void WidgetsCreator::onDeactivate()
    {
        for (const auto& widgetContext : m_WidgetContexts)
        {
            DeactivateEngineObject(widgetContext.value);
        }

        Super::onDeactivate();
    }

    void WidgetsCreator::onClear()
    {
        for (const auto& widgetContext : m_WidgetContexts)
        {
            ClearEngineObject(widgetContext.value);
        }
        for (const auto& widget : m_Widgets)
        {
            ClearEngineObject(widget);
        }
        for (const auto& widgetContext : m_WidgetContexts)
        {
            delete widgetContext.value;
        }
        for (const auto& widget : m_Widgets)
        {
            delete widget;
        }
        m_Widgets.clear();
        m_WidgetContexts.clear();

        Super::onClear();
    }

    WidgetContext* WidgetsCreator::createWidgetContext(const RenderContext& renderContext)
    {
        if (!isInitialized())
        {
            JUTILS_LOG(error, JSTR("Widgets creator not initialized"));
            return nullptr;
        }

        if ((renderContext.renderTarget == nullptr) || !renderContext.renderTarget->isRenderStageIndexValid(renderContext.renderStageIndex))
        {
            JUTILS_LOG(warning, JSTR("Invalid render context"));
            return nullptr;
        }

        if (m_WidgetContexts.find(renderContext) != nullptr)
        {
            JUTILS_LOG(warning, JSTR("Widget context already created"));
            return nullptr;
        }

        WidgetContext* widgetContext = m_WidgetContexts.add(renderContext, getEngine()->createObject1<WidgetContext>());
        widgetContext->m_ParentWidgetsCreator = this;
        widgetContext->m_RenderContext = renderContext;
        InitializeEngineObject(widgetContext);
        if (isActive())
        {
            ActivateEngineObject(widgetContext);
        }
        return widgetContext;
    }
    void WidgetsCreator::destroyWidgetContext(WidgetContext* widgetContext)
    {
        if (widgetContext != nullptr)
        {
            const RenderContext renderContext = widgetContext->getRenderContext();
	        if (m_WidgetContexts.contains(renderContext))
	        {
                ClearEngineObject(widgetContext);
                delete widgetContext;
	            m_WidgetContexts.remove(renderContext);
	        }
        }
    }

    Widget* WidgetsCreator::createWidget(const EngineSubclass<Widget>& widgetClass)
    {
        if (!isInitialized())
        {
            JUTILS_LOG(error, JSTR("Widgets creator not initialized"));
            return nullptr;
        }

        Widget* widget = getEngine()->createObject1<Widget>(widgetClass);
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
}
