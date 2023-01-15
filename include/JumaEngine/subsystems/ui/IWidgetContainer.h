// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"

#include <jutils/math/vector2.h>

namespace JumaEngine
{
    class Widget;

    class IWidgetContainer
    {
    protected:
        IWidgetContainer() = default;
    public:
        virtual ~IWidgetContainer() = default;

    protected:

        static void SetParentForChildWidget(Widget* childWidget, Widget* parentWidget);
        static void SetLocationForChildWidget(Widget* childWidget, const math::vector2& location, const math::vector2& sizeMin, 
            const math::vector2& sizeMax);
    };
}
