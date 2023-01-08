// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "Widget.h"
#include "../../ILogicObjectOwner.h"

#include <jutils/jarray.h>

namespace JumaEngine
{
    class OverlayWidget : public Widget, public ILogicObjectOwner
    {
        JUMAENGINE_CLASS(OverlayWidget, Widget)

    public:
        OverlayWidget() = default;
        virtual ~OverlayWidget() override = default;

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<Widget, T>)>
        T* addWidget()
            { return dynamic_cast<T*>(this->addWidget(T::GetClassStatic())); }
        Widget* addWidget(const EngineSubclass<Widget>& widgetClass);

    protected:

        virtual void onInitialized() override;
        virtual void onLogicStarted() override;
        virtual void onUpdate(float deltaTime) override;
        virtual void onPostUpdate() override;
        virtual void onLogicStopping() override;
        virtual void onDestroying() override;

    private:

        jarray<Widget*> m_Widgets;
    };
}
