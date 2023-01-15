// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/Widget.h"

namespace JumaEngine
{
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
                m_ParentWidget = nullptr;
                onDetachedFromParent.call(this);
            }

            if (widget != nullptr)
            {
                m_ParentWidget = widget;
                m_ParentWidget->onWidgetContextChanged.bind(this, &Widget::onParentWidgetContextChanged);
                setWidgetContext(m_ParentWidget->getWidgetContext());
            }
            else
            {
                setWidgetContext(nullptr);
            }
        }
    }
}
