// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/OverlayWidget.h"

namespace JumaEngine
{
    void OverlayWidget::addWidget(Widget* widget)
    {
        if (!isDestroyed() && (widget != nullptr))
        {
            m_Widgets.add(widget);
            SetParentForChildWidget(widget, this);
        }
    }
}
