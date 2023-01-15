// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../../LogicObject.h"

#include <jutils/jdelegate_multicast.h>

namespace JumaEngine
{
    class IWidgetContainer;
    class Widget;
    class WidgetContext;

    JUTILS_CREATE_MULTICAST_DELEGATE1(OnWidgetEvent, Widget*, widget);

    class Widget : public LogicObject
    {
        JUMAENGINE_CLASS_ABSTRACT(Widget, LogicObject)

        friend IWidgetContainer;
        friend WidgetContext;

    public:
        Widget() = default;
        virtual ~Widget() override = default;

        OnWidgetEvent onWidgetContextChanged;
        OnWidgetEvent onDetachedFromParent;


        WidgetContext* getWidgetContext() const { return m_WidgetContext; }
        Widget* getParentWidget() const { return m_ParentWidget; }

    private:

        WidgetContext* m_WidgetContext = nullptr;
        Widget* m_ParentWidget = nullptr;


        void setWidgetContext(WidgetContext* widgetContext);
        void onParentWidgetContextChanged(Widget* parentWidget);

        void setParentWidget(Widget* widget);
    };
}
