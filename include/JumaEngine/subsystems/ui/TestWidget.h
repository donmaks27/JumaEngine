// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "Widget.h"

#include <jutils/math/vector2.h>

namespace JumaEngine
{
    class Material;

    class TestWidget final : public Widget
    {
        JUMAENGINE_CLASS(TestWidget, Widget)

    public:
        TestWidget() = default;
        virtual ~TestWidget() override = default;
        
        void setDepth(const float depth) { m_Depth = depth; }

        virtual math::vector2 getWidgetRenderLocation() const override;
        
    protected:

        virtual void onInitialized() override;
        virtual void onUpdate(float deltaTime) override;
        virtual void onPreRender() override;
        virtual void onDestroying() override;

        virtual void recalculateWidetSize() override;

    private:

        Material* m_Material = nullptr;

        math::vector2 m_CursorLocation = { 0.0f, 0.0f };
        math::vector2 m_CursorSize = { 1.0f, 1.0f };
        float m_Depth = 0.0f;
        bool m_CursorVisible = true;


        void updateMaterial() const;
    };
}
