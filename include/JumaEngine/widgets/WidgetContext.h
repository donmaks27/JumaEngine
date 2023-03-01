// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineObject.h"
#include "../engine/EngineObjectOwner.h"

#include "../render/RenderContext.h"

namespace JumaEngine
{
    class WidgetsCreator;
    class Widget;
    class WidgetContext;
    
    class WidgetContext final : public EngineObject, public EngineObjectOwner
    {
        JUMAENGINE_CLASS(WidgetContext, EngineObject)

        friend WidgetsCreator;

    public:
        WidgetContext() = default;
        virtual ~WidgetContext() override = default;

        WidgetsCreator* getWidgetsCreator() const { return m_ParentWidgetsCreator; }
        const RenderContext& getRenderContext() const { return m_RenderContext; }
        JumaRE::RenderTarget* getRenderTarget() const { return m_RenderContext.renderTarget; }

        const EngineObjectPtr<Widget>& getRootWidget() const { return m_RootWidget; }
        void setRootWidget(EngineObjectPtr<Widget> widget);

    protected:
        
        virtual void onActivated() override;
        virtual void onUpdate(float deltaTime) override;
        virtual void onPreRender() override;
        virtual void onDeactivate() override;
        virtual void onClear() override;

    private:

        WidgetsCreator* m_ParentWidgetsCreator = nullptr;
        RenderContext m_RenderContext;

        EngineObjectPtr<Widget> m_RootWidget = nullptr;


        void onRootWidgetContextChanging(Widget* widget) { setRootWidget(nullptr); }
        void onRootWidgetDestroying(EngineObject* widget) { setRootWidget(nullptr); }
    };
}
