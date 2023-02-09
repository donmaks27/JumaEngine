// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

#include "../render/RenderContext.h"

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
        const RenderContext& getRenderContext() const { return m_RenderContext; }
        JumaRE::RenderTarget* getRenderTarget() const { return m_RenderContext.renderTarget; }

        Widget* getRootWidget() const { return m_RootWidget; }

    private:

        WidgetsCreator* m_ParentWidgetsCreator = nullptr;
        RenderContext m_RenderContext;

        Widget* m_RootWidget = nullptr;
    };
}
