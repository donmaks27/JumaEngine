// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "Widget.h"

#include <jutils/math/vector4.h>

namespace JumaEngine
{
    class TextureBase;
    class Material;

    class ImageWidget final : public Widget
    {
        JUMAENGINE_CLASS(ImageWidget, Widget)

    public:
        ImageWidget() = default;
        virtual ~ImageWidget() override = default;

        bool shouldUseSolidColor() const { return m_UseSolidColor; }
        math::vector4 getColor() const { return m_MaterialColor; }
        const EngineObjectPtr<TextureBase>& getTexture() const { return m_MaterialTexture; }
        math::vector2 getTextureOffset() const { return m_MaterialTextureOffset; }
        math::vector2 getTextureScale() const { return m_MaterialTextureScale; }

        void setUsingSolidColor(bool useSolidColor);
        void setColor(const math::vector4& color);
        void setTexture(EngineObjectPtr<TextureBase> texture);
        void setTextureOffset(const math::vector2& value);
        void setTextureScale(const math::vector2& value);

        void setDepth(float depth);

    protected:

        virtual void onInitialized() override;
        virtual void onPreRender() override;
        virtual void onClear() override;

        virtual void recalculateWidetSize() override;

    private:

        EngineObjectPtr<Material> m_WidgetMaterial = nullptr;

        math::vector4 m_MaterialColor = { 1.0f, 1.0f, 1.0f, 1.0f };

        EngineObjectPtr<TextureBase> m_MaterialTexture = nullptr;
        math::vector2 m_MaterialTextureOffset = { 0.0f, 0.0f };
        math::vector2 m_MaterialTextureScale = { 1.0f, 1.0f };

        bool m_UseSolidColor = false;


        void recreateMaterial();

        void updateMaterialColorParams() const;
        void updateMaterialLocationParams() const;
    };
}
