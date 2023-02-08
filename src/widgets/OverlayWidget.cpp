// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/widgets/OverlayWidget.h"

namespace JumaEngine
{
    void OverlayWidget::addWidget(Widget* widget)
    {
        if (!isDestroyed() && (widget != nullptr) && (getWidgetsCreator() == widget->getWidgetsCreator()))
        {
            m_Widgets.add(widget);
            SetParentForChildWidget(widget, this);

            if (isLogicActive())
            {
                StartLogicObject(widget);
            }
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

    void OverlayWidget::onPreRender()
    {
        Super::onPreRender();

        for (const auto& widget : m_Widgets)
        {
            PreRenderLogicObject(widget);
        }
    }
    void OverlayWidget::recalculateWidetSize()
    {
        const math::box2 bounds = getWidgetBounds();
        m_WidgetRenderSize = bounds.v1 - bounds.v0;
	    for (const auto& widget : m_Widgets)
	    {
		    RecalculateChildWidgetSize(widget, getWidgetBounds(), WidgetAlignmentH::Fill, WidgetAlignmentV::Fill);
	    }
    }
}
