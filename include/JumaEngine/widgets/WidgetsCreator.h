﻿// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineObject.h"
#include "../engine/IEngineObjectOwner.h"

#include <JumaRE/RenderTarget.h>
#include <jutils/jmap.h>

#include "WidgetContext.h"

namespace JumaEngine
{
    class Widget;

    class WidgetsCreator final : public EngineObject, public IEngineObjectOwner
    {
        JUMAENGINE_CLASS(WidgetsCreator, EngineObject)

    public:
        WidgetsCreator() = default;
        virtual ~WidgetsCreator() override = default;

        WidgetContext* createWidgetContext(JumaRE::RenderTarget* renderTarget);
        void destroyWidgetContext(WidgetContext* widgetContext);

        Widget* createWidget(const EngineSubclass<Widget>& widgetClass);
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<Widget, T>)>
        T* createWidget() { return dynamic_cast<T*>(this->createWidget(T::GetClassStatic())); }
        bool destroyWidget(Widget* widget, bool destroyChildWidgets = false);

        void setRootWidget(WidgetContext* widgetContext, Widget* widget);

    protected:

        virtual void onInitialized() override;
        virtual void onActivated() override;
        virtual void onUpdate(float deltaTime) override;
        virtual void onPreRender() override;
        virtual void onDeactivate() override;
        virtual void onClear() override;

    private:

        jmap<JumaRE::RenderTarget*, WidgetContext> m_WidgetContexts;
        jarray<Widget*> m_Widgets;
    };
}
