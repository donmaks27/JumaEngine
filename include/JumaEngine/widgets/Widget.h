// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineObject.h"

#include <jutils/jdelegate_multicast.h>
#include <jutils/math/box2.h>
#include <jutils/math/vector2.h>

#include "WidgetAlignment.h"

namespace JumaEngine
{
    class Widget;
    class WidgetContext;
    class WidgetsCreator;
    class WidgetContainer;

    JUTILS_CREATE_MULTICAST_DELEGATE1(OnWidgetEvent, Widget*, widget);

    class Widget : public EngineObject
    {
        JUMAENGINE_CLASS_ABSTRACT(Widget, EngineObject)

        friend WidgetsCreator;
        friend WidgetContainer;
        friend WidgetContext;

    public:
        Widget() = default;
        virtual ~Widget() override = default;

        OnWidgetEvent onWidgetContextChanged;
        OnWidgetEvent onDetachedFromParent;


        WidgetsCreator* getWidgetsCreator() const { return m_ParentWidgetsCreator; }
        EngineObjectWeakPtr<WidgetContext> getWidgetContext() const;
        EngineObjectWeakPtr<Widget> getParentWidget() const { return EngineObjectWeakPtr(m_ParentWidget); }
        virtual jarray<Widget*> getChildWidgets() const { return {}; }
        
        const math::box2& getWidgetBounds() const { return m_WidgetBounds; }
        WidgetAlignmentH getWidgetAlignmentH() const { return m_WidgetAlignmentH; }
        WidgetAlignmentV getWidgetAlignmentV() const { return m_WidgetAlignmentV; }

        math::vector2 getWidgetRenderSize() const { return m_WidgetRenderSize; }
        virtual math::vector2 getWidgetRenderLocation() const;

        void destroyWidget(bool destroyChildWidgets);

    protected:

        math::vector2 m_WidgetRenderSize = { 0.0f, 0.0f };


        virtual void onClear() override;

        WidgetContext* getWidgetContextPtr() const { return m_WidgetContext; }
        
        virtual void recalculateWidetSize() { m_WidgetRenderSize = { 0.0f, 0.0f }; }
        
    private:

        WidgetsCreator* m_ParentWidgetsCreator = nullptr;
        WidgetContext* m_WidgetContext = nullptr;
        Widget* m_ParentWidget = nullptr;
        
        math::box2 m_WidgetBounds = { { 0.0f, 0.0f }, { 1.0f, 1.0f } };
        WidgetAlignmentH m_WidgetAlignmentH = WidgetAlignmentH::Left;
        WidgetAlignmentV m_WidgetAlignmentV = WidgetAlignmentV::Top;


        void setWidgetContext(WidgetContext* widgetContext);

        void setParentWidget(Widget* widget);
        void onParentWidgetContextChanged(Widget* parentWidget) { setWidgetContext(m_ParentWidget->getWidgetContextPtr()); }
        void onParentWidgetDestroying(EngineObject* widget) { setParentWidget(nullptr); }

        void setWidgetBounds(const math::box2& bounds, WidgetAlignmentH alignmentH, WidgetAlignmentV alignmentV);
    };
}
