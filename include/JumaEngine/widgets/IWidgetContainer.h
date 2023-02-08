// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../IEngineObjectOwner.h"

#include <jutils/math/box2.h>

#include "WidgetAlignment.h"

namespace JumaEngine
{
    class Widget;

    class IWidgetContainer : public IEngineObjectOwner
    {
    protected:
        IWidgetContainer() = default;
    public:
        virtual ~IWidgetContainer() override = default;

    protected:

        static void SetParentForChildWidget(Widget* childWidget, Widget* parentWidget);
        static void RecalculateChildWidgetSize(Widget* childWidget, const math::box2& bounds, WidgetAlignmentH alignmentH, WidgetAlignmentV alignmentV);
    };
}
