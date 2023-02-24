// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/widgets/WidgetContainer.h"

#include "JumaEngine/widgets/Widget.h"

namespace JumaEngine
{
    void WidgetContainer::SetParentForChildWidget(Widget* childWidget, Widget* parentWidget)
    {
        childWidget->setParentWidget(parentWidget);
    }
    void WidgetContainer::RecalculateChildWidgetSize(Widget* childWidget, const math::box2& bounds, const WidgetAlignmentH alignmentH, 
        const WidgetAlignmentV alignmentV)
    {
        childWidget->setWidgetBounds(bounds, alignmentH, alignmentV);
        childWidget->recalculateWidetSize();
    }
}
