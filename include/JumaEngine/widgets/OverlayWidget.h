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

        void addWidget(EngineObjectPtr<Widget> widget);
        void clearChildWidgets();

    protected:

        virtual void onActivated() override;
        virtual void onUpdate(float deltaTime) override;
        virtual void onPreRender() override;
        virtual void onDeactivate() override;
        virtual void onClear() override;
        
        virtual jarray<Widget*> getChildWidgetPtrs() const override;

        virtual void recalculateWidetSize() override;

    private:

        jarray<EngineObjectPtr<Widget>> m_Widgets;


        void onChildWidgetDestroying(EngineObject* object) { detachChildWidget(dynamic_cast<Widget*>(object)); }
        void detachChildWidget(Widget* widget);
    };
}
