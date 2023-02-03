﻿// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "Widget.h"

#include <JumaRE/texture/TextureBase.h>
#include <jutils/math/vector4.h>

namespace JumaEngine
{
    class Material;

    class ImageWidget final : public Widget
    {
        JUMAENGINE_CLASS(ImageWidget, Widget)

    public:
        ImageWidget() = default;
        virtual ~ImageWidget() override = default;

        bool shouldUseSolidColor() const { return m_UseSolidColor; }
        math::vector4 getColor() const { return m_MaterialColor; }
        JumaRE::TextureBase* getTexture() const { return m_MaterialTexture; }

        void setUsingSolidColor(bool useSolidColor);
        void setColor(const math::vector4& color);
        void setTexture(JumaRE::TextureBase* texture);

        void setDepth(float depth);

    protected:

        virtual void onInitialized() override;
        virtual void onPreRender() override;
        virtual void onDestroying() override;

        virtual void recalculateWidetSize() override;

    private:

        Material* m_WidgetMaterial = nullptr;

        math::vector4 m_MaterialColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        JumaRE::TextureBase* m_MaterialTexture = nullptr;
        bool m_UseSolidColor = false;


        void recreateMaterial();

        void updateMaterialColorParams() const;
        void updateMaterialLocationParams() const;
    };
}
