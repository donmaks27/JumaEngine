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

        void setMaterial(Material* material);
        Material* getMaterial() const { return m_Material; }

        void setLocation(const math::vector2& location) { m_Location = location; }
        void setOffset(const math::vector2& offset) { m_Offset = offset; }
        void setSize(const math::vector2& size) { m_Size = size; }
        void setDepth(const float depth) { m_Depth = depth; }
        void setVisibility(const bool visible) { m_Visible = visible; }

        bool isVisible() const { return m_Visible; }

    protected:

        virtual void onUpdate(float deltaTime) override;
        virtual void onPostUpdate() override;

    private:

        Material* m_Material = nullptr;

        math::vector2 m_Location = { 0.0f, 0.0f };
        math::vector2 m_Offset = { 0.0f, 0.0f };
        math::vector2 m_Size = { 1.0f, 1.0f };
        float m_Depth = 0.0f;
        bool m_Visible = true;


        void updateMaterial() const;
    };
}
