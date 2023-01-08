// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../../LogicObject.h"
#include "../../ILogicObjectOwner.h"

#include <JumaRE/RenderTarget.h>

namespace JumaEngine
{
    class GameInstance;
    class Widget;

    class WidgetContainer final : public LogicObject, public ILogicObjectOwner
    {
        JUMAENGINE_CLASS(WidgetContainer, LogicObject)

        friend GameInstance;

    public:
        WidgetContainer() = default;
        virtual ~WidgetContainer() override = default;

        JumaRE::RenderTarget* getRenderTarget() const { return m_RenderTarget; }

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<Widget, T>)>
        T* setRootWidget()
            { return dynamic_cast<T*>(this->setRootWidget(T::GetClassStatic())); }
        Widget* setRootWidget(const EngineSubclass<Widget>& widgetClass);
        Widget* getRootWidget() const { return m_RootWidget; }

    protected:

        virtual void onInitialized() override;
        virtual void onLogicStarted() override;
        virtual void onUpdate(float deltaTime) override;
        virtual void onPostUpdate() override;
        virtual void onLogicStopping() override;
        virtual void onDestroying() override;

    private:

        JumaRE::RenderTarget* m_RenderTarget = nullptr;

        Widget* m_RootWidget = nullptr;

        
        void setRenderTarget(JumaRE::RenderTarget* renderTarget) { m_RenderTarget = renderTarget; }
    };
}
