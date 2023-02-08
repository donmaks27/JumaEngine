// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/widgets/IWidgetContainer.h"

#include "JumaEngine/widgets/Widget.h"

namespace JumaEngine
{
    void IWidgetContainer::SetParentForChildWidget(Widget* childWidget, Widget* parentWidget)
    {
        childWidget->setParentWidget(parentWidget);
    }
    void IWidgetContainer::RecalculateChildWidgetSize(Widget* childWidget, const math::box2& bounds, const WidgetAlignmentH alignmentH, 
        const WidgetAlignmentV alignmentV)
    {
        childWidget->setWidgetBounds(bounds, alignmentH, alignmentV);
        childWidget->recalculateWidetSize();
    }
}
