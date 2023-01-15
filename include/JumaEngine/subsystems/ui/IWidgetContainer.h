// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"

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
    };
}
