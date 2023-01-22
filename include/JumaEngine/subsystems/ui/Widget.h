// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../../LogicObject.h"

#include <jutils/jdelegate_multicast.h>
#include <jutils/math/vector2.h>

namespace JumaEngine
{
    class Widget;
    class WidgetContext;
    class WidgetsCreator;
    class IWidgetContainer;

    JUTILS_CREATE_MULTICAST_DELEGATE1(OnWidgetEvent, Widget*, widget);

    class Widget : public LogicObject
    {
        JUMAENGINE_CLASS_ABSTRACT(Widget, LogicObject)

        friend WidgetsCreator;
        friend IWidgetContainer;

    public:
        Widget() = default;
        virtual ~Widget() override = default;

        OnWidgetEvent onWidgetContextChanged;
        OnWidgetEvent onDetachedFromParent;
        OnWidgetEvent onWidgetDestroying;


        WidgetsCreator* getWidgetsCreator() const { return m_ParentWidgetsCreator; }
        WidgetContext* getWidgetContext() const { return m_WidgetContext; }
        Widget* getParentWidget() const { return m_ParentWidget; }

        math::vector2 getLocation() const { return m_WidgetLocation; }
        math::vector2 getSize() const { return m_WidgetActualSize; }

    protected:

        virtual void onDestroying() override;


        math::vector2 getMinSize() const { return m_WidgetSizeMin; }
        math::vector2 getMaxSize() const { return m_WidgetSizeMax; }

        void setActualSize(const math::vector2& size) { m_WidgetActualSize = size; }

    private:

        WidgetsCreator* m_ParentWidgetsCreator = nullptr;
        WidgetContext* m_WidgetContext = nullptr;
        Widget* m_ParentWidget = nullptr;

        math::vector2 m_WidgetLocation = { 0.0f, 0.0f };
        math::vector2 m_WidgetSizeMin = { 0.0f, 0.0f };
        math::vector2 m_WidgetSizeMax = { 0.0f, 0.0f };
        math::vector2 m_WidgetActualSize = { 0.0f, 0.0f };


        void setWidgetContext(WidgetContext* widgetContext);
        void onParentWidgetContextChanged(Widget* parentWidget);

        void setParentWidget(Widget* widget);
        void onParentWidgetDestroying(Widget* widget);

        void setWidgetLocation(const math::vector2& location, const math::vector2& sizeMin, const math::vector2& sizeMax);
    };
}
