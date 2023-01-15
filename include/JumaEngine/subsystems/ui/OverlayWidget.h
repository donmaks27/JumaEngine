// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "Widget.h"
#include "IWidgetContainer.h"
#include "../../ILogicObjectOwner.h"

#include <jutils/jarray.h>

namespace JumaEngine
{
    class OverlayWidget : public Widget, public IWidgetContainer, public ILogicObjectOwner
    {
        JUMAENGINE_CLASS(OverlayWidget, Widget)

    public:
        OverlayWidget() = default;
        virtual ~OverlayWidget() override = default;

        void addWidget(Widget* widget);

    private:

        jarray<Widget*> m_Widgets;
    };
}
