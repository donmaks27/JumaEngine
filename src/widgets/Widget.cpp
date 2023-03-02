// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/widgets/Widget.h"

#include "JumaEngine/engine/Engine.h"
#include "JumaEngine/widgets/WidgetContext.h"
#include "JumaEngine/widgets/WidgetsCreator.h"

namespace JumaEngine
{
    void Widget::onClear()
    {
        setParentWidget(nullptr);
        setWidgetContext(nullptr);
        m_ParentWidgetsCreator = nullptr;

        Super::onClear();
    }

    void Widget::setWidgetContext(WidgetContext* widgetContext)
    {
        if (m_WidgetContext != widgetContext)
        {
            m_WidgetContext = widgetContext;
            onWidgetContextChanged.call(this);
        }
    }

    EngineObjectPtr<WidgetContext> Widget::getWidgetContext() const
    {
        return EngineObjectPtr(getWidgetContextPtr());
    }

    void Widget::setParentWidget(Widget* widget)
    {
        if (m_ParentWidget != widget)
        {
            if (m_ParentWidget != nullptr)
            {
                m_ParentWidget->onWidgetContextChanged.unbind(this, &Widget::onParentWidgetContextChanged);
                m_ParentWidget->onDestroying.unbind(this, &Widget::onParentWidgetDestroying);
                m_ParentWidget = nullptr;
                onDetachedFromParent.call(this);
            }

            if (widget != nullptr)
            {
                m_ParentWidget = widget;
                m_ParentWidget->onWidgetContextChanged.bind(this, &Widget::onParentWidgetContextChanged);
                m_ParentWidget->onDestroying.bind(this, &Widget::onParentWidgetDestroying);
                setWidgetContext(m_ParentWidget->getWidgetContextPtr());
            }
            else
            {
                setWidgetContext(nullptr);
            }
        }
    }
    
    void Widget::setWidgetBounds(const math::box2& bounds, const WidgetAlignmentH alignmentH, const WidgetAlignmentV alignmentV)
    {
        m_WidgetBounds = bounds;
        m_WidgetAlignmentH = alignmentH;
        m_WidgetAlignmentV = alignmentV;
    }
    math::vector2 Widget::getWidgetRenderLocation() const
	{
        math::vector2 location;
        switch (m_WidgetAlignmentH)
        {
        case WidgetAlignmentH::Left: 
        case WidgetAlignmentH::Fill:
            location.x = m_WidgetBounds.v0.x;
        	break;
        case WidgetAlignmentH::Center: 
            location.x = m_WidgetBounds.v0.x + ((m_WidgetBounds.v1.x - m_WidgetBounds.v0.x - m_WidgetRenderSize.x) / 2);
        	break;
        case WidgetAlignmentH::Right: 
            location.x = m_WidgetBounds.v1.x - m_WidgetRenderSize.x;
            break;
        default: ;
        }
        switch (m_WidgetAlignmentV)
        {
        case WidgetAlignmentV::Top:
        case WidgetAlignmentV::Fill:
            location.y = m_WidgetBounds.v0.y;
        	break;
        case WidgetAlignmentV::Center: 
            location.y = m_WidgetBounds.v0.y + ((m_WidgetBounds.v1.y - m_WidgetBounds.v0.y - m_WidgetRenderSize.y) / 2);
            break;
        case WidgetAlignmentV::Bottom: 
            location.y = m_WidgetBounds.v1.y - m_WidgetRenderSize.y;
        	break;
        default: ;
        }
        return location;
	}

    void Widget::destroy(const bool destroyChildWidgets)
    {
        if (destroyChildWidgets)
        {
            for (const auto& widget : getChildWidgetPtrs())
            {
                widget->destroy(true);
            }
        }
        getWidgetsCreator()->destroyWidget(this);
    }
}
