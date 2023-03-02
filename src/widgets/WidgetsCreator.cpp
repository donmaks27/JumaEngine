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
            widgetContext.value->onDestroying.unbind(this, &WidgetsCreator::onWidgetContextDestroying);
            widgetContext.value->destroy();
        }
        for (const auto& widget : m_Widgets)
        {
            widget->onDestroying.unbind(this, &WidgetsCreator::onWidgetDestroying);
            widget->destroy(false);
        }
        m_Widgets.clear();
        m_WidgetContexts.clear();

        Super::onClear();
    }

    EngineObjectPtr<WidgetContext> WidgetsCreator::createWidgetContext(const RenderContext& renderContext)
    {
        if (!isInitialized())
        {
            JUTILS_LOG(error, JSTR("WidgetsCreator is not initialized"));
            return nullptr;
        }

        if ((renderContext.renderTarget == nullptr) || !renderContext.renderTarget->isRenderStageIndexValid(renderContext.renderStageIndex))
        {
            JUTILS_LOG(warning, JSTR("Invalid render context"));
            return nullptr;
        }

        if (m_WidgetContexts.contains(renderContext))
        {
            JUTILS_LOG(warning, JSTR("WidgetContext for this RenderContext is already created"));
            return nullptr;
        }

        EngineObjectPtr<WidgetContext> widgetContext = getEngine()->createObject<WidgetContext>();
        if (widgetContext == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create WidgetContext"));
            return nullptr;
        }

        m_WidgetContexts.add(renderContext, widgetContext.get());
        widgetContext->m_ParentWidgetsCreator = this;
        widgetContext->m_RenderContext = renderContext;
        widgetContext->onDestroying.bind(this, &WidgetsCreator::onWidgetContextDestroying);

        InitializeEngineObject(widgetContext.get());
        if (isActive())
        {
            ActivateEngineObject(widgetContext.get());
        }
        return widgetContext;
    }
    void WidgetsCreator::onWidgetContextDestroying(EngineObject* object)
    {
        WidgetContext* widgetContext = dynamic_cast<WidgetContext*>(object);
        widgetContext->onDestroying.unbind(this, &WidgetsCreator::onWidgetContextDestroying);
        m_WidgetContexts.remove(widgetContext->getRenderContext());
    }

    EngineObjectPtr<Widget> WidgetsCreator::createWidget(const EngineSubclass<Widget>& widgetClass)
    {
        if (!isInitialized())
        {
            JUTILS_LOG(error, JSTR("Widgets creator not initialized"));
            return nullptr;
        }

        EngineObjectPtr<Widget> widget = getEngine()->createObject(widgetClass);
        if (widget == nullptr)
        {
            return nullptr;
        }

        m_Widgets.add(widget.get());
        widget->m_ParentWidgetsCreator = this;
        widget->onDestroying.bind(this, &WidgetsCreator::onWidgetDestroying);

        InitializeEngineObject(widget.get());
        return widget;
    }
    void WidgetsCreator::destroyWidget(Widget* widget)
    {
        ClearEngineObject(widget);
    }
    void WidgetsCreator::onWidgetDestroying(EngineObject* widget)
    {
        widget->onDestroying.unbind(this, &WidgetsCreator::onWidgetDestroying);
        m_Widgets.remove(dynamic_cast<Widget*>(widget));
    }
}
