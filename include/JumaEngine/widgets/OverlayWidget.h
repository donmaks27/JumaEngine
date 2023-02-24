// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "Widget.h"
#include "WidgetContainer.h"

#include <jutils/jarray.h>

namespace JumaEngine
{
    class OverlayWidget : public Widget, public WidgetContainer
    {
        JUMAENGINE_CLASS(OverlayWidget, Widget)

    public:
        OverlayWidget() = default;
        virtual ~OverlayWidget() override = default;

        virtual jarray<Widget*> getChildWidgets() const override { return m_Widgets; }

        void addWidget(Widget* widget);

    protected:

        virtual void onActivated() override;
        virtual void onUpdate(float deltaTime) override;
        virtual void onPreRender() override;

        virtual void recalculateWidetSize() override;

    private:

        jarray<Widget*> m_Widgets;
    };
}
