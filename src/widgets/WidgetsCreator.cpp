﻿// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/widgets/WidgetsCreator.h"

#include "JumaEngine/engine/Engine.h"
#include "JumaEngine/widgets/Widget.h"
#include "JumaEngine/widgets/WidgetContext.h"

namespace JumaEngine
{
    void WidgetsCreator::onActivated()
    {
        Super::onActivated();

        for (const auto& widgetContext : m_WidgetContexts.values())
        {
            ActivateEngineObject(widgetContext);
        }
    }
    void WidgetsCreator::onUpdate(const float deltaTime)
    {
        Super::onUpdate(deltaTime);

        for (const auto& widgetContext : m_WidgetContexts.values())
        {
            UpdateEngineObject(widgetContext, deltaTime);
        }
    }
    void WidgetsCreator::onPreRender()
    {
        Super::onPreRender();

        for (const auto& widgetContext : m_WidgetContexts.values())
        {
            PreRenderEngineObject(widgetContext);
        }
    }
    void WidgetsCreator::onDeactivate()
    {
        for (const auto& widgetContext : m_WidgetContexts.values())
        {
            DeactivateEngineObject(widgetContext);
        }

        Super::onDeactivate();
    }
    void WidgetsCreator::onClear()
    {
        for (const auto& widgetContext : m_WidgetContexts.values())
        {
            widgetContext->onDestroying.unbind(this, &WidgetsCreator::onWidgetContextDestroying);
            widgetContext->destroy();
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

        const EngineObjectPtr<RenderTarget>& renderTarget = renderContext.getRenderTarget();
        if ((renderTarget == nullptr) || !renderTarget->getRenderTarget()->isRenderStageIndexValid(renderContext.renderStageIndex))
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
    void WidgetsCreator::onWidgetContextDestroying(EngineContextObject* object)
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
    void WidgetsCreator::onWidgetDestroying(EngineContextObject* widget)
    {
        widget->onDestroying.unbind(this, &WidgetsCreator::onWidgetDestroying);
        m_Widgets.remove(reinterpret_cast<Widget*>(widget));
    }
}
