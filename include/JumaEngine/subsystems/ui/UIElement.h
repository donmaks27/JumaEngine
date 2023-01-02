// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../../EngineContextObject.h"

#include "../shaders/Material.h"

namespace JumaEngine
{
    class UIElement : public EngineContextObject
    {
        JUMAENGINE_CLASS(UIElement, EngineContextObject)

    public:
        UIElement() = default;
        virtual ~UIElement() override;

        void setMaterial(Material* material);
        Material* getMaterial() const { return m_Material; }

        void setLocation(const math::vector2& location) { m_Location = location; }
        void setOffset(const math::vector2& offset) { m_Offset = offset; }
        void setSize(const math::vector2& size) { m_Size = size; }
        void setDepth(const float depth) { m_Depth = depth; }
        void updateMaterial();

    private:

        Material* m_Material = nullptr;

        math::vector2 m_Location = { 0.0f, 0.0f };
        math::vector2 m_Offset = { 0.0f, 0.0f };
        math::vector2 m_Size = { 1.0f, 1.0f };
        float m_Depth = 0.0f;


        void clearWidget();
    };
}
