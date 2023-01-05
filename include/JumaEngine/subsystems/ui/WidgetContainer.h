// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../../LogicObject.h"

#include <JumaRE/RenderTarget.h>

namespace JumaEngine
{
    class Widget;

    class WidgetContainer final : public LogicObject
    {
        JUMAENGINE_CLASS(WidgetContainer, LogicObject)

    public:
        WidgetContainer() = default;
        virtual ~WidgetContainer() override = default;

        void setRenderTarget(JumaRE::RenderTarget* renderTarget);
        JumaRE::RenderTarget* getRenderTarget() const { return m_RenderTarget; }

        void setRootWidget(const EngineSubclass<Widget>& widgetClass);
        Widget* getRootWidget() const { return m_RootWidget; }

        virtual bool initLogic() override;
        virtual void startLogic() override;
        virtual void clearLogic() override;

        virtual void update(float deltaTime) override;
        virtual void postUpdate() override;

    private:

        JumaRE::RenderTarget* m_RenderTarget = nullptr;

        Widget* m_RootWidget = nullptr;

        bool m_LogicInitialized = false;
        bool m_LogicStarted = false;


        bool initRootWidget();
    };
}
