// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/WidgetContext.h"

#include "JumaEngine/subsystems/ui/Widget.h"

namespace JumaEngine
{
    void WidgetContext::setRootWidget(Widget* widget)
    {
        if (m_RootWidget == widget)
        {
            return;
        }

        if (m_RootWidget != nullptr)
        {
            m_RootWidget->setWidgetContext(nullptr);
        }
        m_RootWidget = widget;
        if (m_RootWidget != nullptr)
        {
            m_RootWidget->setParentWidget(nullptr);
            m_RootWidget->setWidgetContext(this);
        }
    }
}
