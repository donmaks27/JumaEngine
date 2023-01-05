// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../../LogicObject.h"

namespace JumaEngine
{
    class WidgetContainer;

    class Widget : public LogicObject
    {
        JUMAENGINE_CLASS_ABSTRACT(Widget, LogicObject)

        friend WidgetContainer;

    public:
        Widget() = default;
        virtual ~Widget() override = default;

        WidgetContainer* getWidgetContainer() const { return m_WidgetContainer; }

    private:

        WidgetContainer* m_WidgetContainer = nullptr;


        void setWidgetContainer(WidgetContainer* widgetContainer);
    };
}
