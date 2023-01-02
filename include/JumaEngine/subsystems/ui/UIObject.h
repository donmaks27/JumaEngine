// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../../EngineContextObject.h"

#include <JumaRE/RenderTarget.h>

namespace JumaEngine
{
    class Material;
    class UIElement;

    class UIObject : public EngineContextObject
    {
        JUMAENGINE_CLASS(UIObject, EngineContextObject)

    public:
        UIObject() = default;
        virtual ~UIObject() override;

        void setRenderTarget(JumaRE::RenderTarget* renderTarget) { m_RenderTarget = renderTarget; }
        UIElement* addElement();

        void update();

    private:

        JumaRE::RenderTarget* m_RenderTarget = nullptr;
        jarray<UIElement*> m_UIElements;
    };
}
