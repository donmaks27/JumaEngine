// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineObject.h"
#include "../engine/EngineObjectOwner.h"

#include <jutils/jmap.h>

#include "WidgetContext.h"

namespace JumaEngine
{
    class Widget;

    class WidgetsCreator final : public EngineObject, public EngineObjectOwner
    {
        JUMAENGINE_CLASS(WidgetsCreator, EngineObject)

        friend Widget;
            
    public:
        WidgetsCreator() = default;
        virtual ~WidgetsCreator() override = default;

        EngineObjectPtr<WidgetContext> createWidgetContext(const RenderContext& renderContext);

        EngineObjectPtr<Widget> createWidget(const EngineSubclass<Widget>& widgetClass);
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<Widget, T>)>
        EngineObjectPtr<T> createWidget() { return this->createWidget(T::GetClassStatic()).template cast<T>(); }
        
    protected:
        
        virtual void onActivated() override;
        virtual void onUpdate(float deltaTime) override;
        virtual void onPreRender() override;
        virtual void onDeactivate() override;
        virtual void onClear() override;

    private:

        jmap<RenderContext, WidgetContext*> m_WidgetContexts;
        jarray<Widget*> m_Widgets;


        void onWidgetContextDestroying(EngineObject* object);

        void destroyWidget(Widget* widget);
        void onWidgetDestroying(EngineObject* widget);
    };
}
