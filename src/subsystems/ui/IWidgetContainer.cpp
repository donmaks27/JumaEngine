// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/IWidgetContainer.h"

#include "JumaEngine/subsystems/ui/Widget.h"

namespace JumaEngine
{
    void IWidgetContainer::SetParentForChildWidget(Widget* childWidget, Widget* parentWidget)
    {
        childWidget->setParentWidget(parentWidget);
    }
    void IWidgetContainer::SetLocationForChildWidget(Widget* childWidget, const math::vector2& location, const math::vector2& sizeMin, 
        const math::vector2& sizeMax)
    {
        childWidget->setWidgetLocation(location, sizeMin, sizeMax);
    }
}
