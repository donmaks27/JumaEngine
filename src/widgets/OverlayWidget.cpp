// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/widgets/OverlayWidget.h"

namespace JumaEngine
{
    void OverlayWidget::onActivated()
    {
        Super::onActivated();

        for (const auto& widget : m_Widgets)
        {
            ActivateEngineObject(widget.get());
        }
    }

    void OverlayWidget::onUpdate(const float deltaTime)
    {
        Super::onUpdate(deltaTime);

        for (const auto& widget : m_Widgets)
        {
            UpdateEngineObject(widget.get(), deltaTime);
        }
    }

    void OverlayWidget::onPreRender()
    {
        Super::onPreRender();

        for (const auto& widget : m_Widgets)
        {
            PreRenderEngineObject(widget.get());
        }
    }
    void OverlayWidget::recalculateWidetSize()
    {
        const math::box2 bounds = getWidgetBounds();
        m_WidgetRenderSize = bounds.v1 - bounds.v0;
	    for (const auto& widget : m_Widgets)
	    {
		    RecalculateChildWidgetSize(widget.get(), getWidgetBounds(), WidgetAlignmentH::Fill, WidgetAlignmentV::Fill);
	    }
    }

    void OverlayWidget::onDeactivate()
    {
        for (const auto& widget : m_Widgets)
        {
            DeactivateEngineObject(widget.get());
        }

        Super::onDeactivate();
    }

    void OverlayWidget::onClear()
    {
        clearChildWidgets();

        Super::onClear();
    }

    jarray<Widget*> OverlayWidget::getChildWidgets() const
    {
        jarray<Widget*> widgets;
        for (const auto& widget : m_Widgets)
        {
            widgets.add(widget.get());
        }
        return widgets;
    }
    
    void OverlayWidget::addWidget(EngineObjectPtr<Widget> widget)
    {
        if (!isDestroyed() && (widget != nullptr) && (getWidgetsCreator() == widget->getWidgetsCreator()))
        {
            const EngineObjectPtr<Widget>& addedWidget = m_Widgets.add(std::move(widget));
            SetParentForChildWidget(addedWidget.get(), this);

            addedWidget->onDestroying.bind(this, &OverlayWidget::onChildWidgetDestroying);
            addedWidget->onDetachedFromParent.bind(this, &OverlayWidget::detachChildWidget);

            if (isActive())
            {
                ActivateEngineObject(addedWidget.get());
            }
        }
    }
    void OverlayWidget::detachChildWidget(Widget* widget)
    {
        widget->onDestroying.unbind(this, &OverlayWidget::onChildWidgetDestroying);
        widget->onDetachedFromParent.unbind(this, &OverlayWidget::detachChildWidget);

        const EngineObjectWeakPtr<EngineObject> pointer(widget);
        for (int32 index = 0; index < m_Widgets.getSize(); index++)
        {
            if (m_Widgets[index] == pointer)
            {
                m_Widgets.removeAt(index);
            }
        }
    }
    void OverlayWidget::clearChildWidgets()
    {
        for (const auto& widget : m_Widgets)
        {
            widget->onDestroying.unbind(this, &OverlayWidget::onChildWidgetDestroying);
            widget->onDetachedFromParent.unbind(this, &OverlayWidget::detachChildWidget);
        }
        m_Widgets.clear();
    }
}
