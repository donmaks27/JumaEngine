// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

#include <JumaRE/RenderTarget.h>

namespace JumaEngine
{
    class WidgetsCreator;
    class Widget;

    class WidgetContext final
    {
        friend WidgetsCreator;

    public:
        WidgetContext() = default;

        WidgetsCreator* getWidgetsCreator() const { return m_ParentWidgetsCreator; }
        JumaRE::RenderTarget* getRenderTarget() const { return m_RenderTarget; }

        Widget* getRootWidget() const { return m_RootWidget; }

    private:

        WidgetsCreator* m_ParentWidgetsCreator = nullptr;
        JumaRE::RenderTarget* m_RenderTarget = nullptr;

        Widget* m_RootWidget = nullptr;
    };
}
