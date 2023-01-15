// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/Widget.h"

namespace JumaEngine
{
    void Widget::onDestroying()
    {
        onWidgetDestroying.call(this);

        Super::onDestroying();
    }

    void Widget::setWidgetContext(WidgetContext* widgetContext)
    {
        if (m_WidgetContext != widgetContext)
        {
            m_WidgetContext = widgetContext;
            onWidgetContextChanged.call(this);
        }
    }
    void Widget::onParentWidgetContextChanged(Widget* parentWidget)
    {
        setWidgetContext(m_ParentWidget->getWidgetContext());
    }

    void Widget::setParentWidget(Widget* widget)
    {
        if (m_ParentWidget != widget)
        {
            if (m_ParentWidget != nullptr)
            {
                m_ParentWidget->onWidgetContextChanged.unbind(this, &Widget::onParentWidgetContextChanged);
                m_ParentWidget->onWidgetDestroying.unbind(this, &Widget::onParentWidgetDestroying);
                m_ParentWidget = nullptr;
                onDetachedFromParent.call(this);
            }

            if (widget != nullptr)
            {
                m_ParentWidget = widget;
                m_ParentWidget->onWidgetContextChanged.bind(this, &Widget::onParentWidgetContextChanged);
                m_ParentWidget->onWidgetDestroying.bind(this, &Widget::onParentWidgetDestroying);
                setWidgetContext(m_ParentWidget->getWidgetContext());
            }
            else
            {
                setWidgetContext(nullptr);
            }
        }
    }
    void Widget::onParentWidgetDestroying(Widget* widget)
    {
        setParentWidget(nullptr);
    }

    void Widget::setWidgetLocation(const math::vector2& location, const math::vector2& sizeMin, const math::vector2& sizeMax)
    {
        m_WidgetLocation = location;
        m_WidgetSizeMin = sizeMin;
        m_WidgetSizeMax = sizeMax;
    }
}
