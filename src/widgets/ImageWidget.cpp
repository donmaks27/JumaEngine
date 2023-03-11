﻿// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/widgets/ImageWidget.h"

#include "JumaEngine/engine/Engine.h"
#include "JumaEngine/assets/AssetsEngineSubsystem.h"
#include "JumaEngine/widgets/WidgetContext.h"

namespace JumaEngine
{
    void ImageWidget::setUsingSolidColor(const bool useSolidColor)
    {
        if (useSolidColor != m_UseSolidColor)
        {
            m_UseSolidColor = useSolidColor;
            recreateMaterial();
        }
    }
    void ImageWidget::setColor(const math::vector4& color)
    {
        m_MaterialColor = color;
        if (shouldUseSolidColor())
        {
            updateMaterialColorParams();
        }
    }
    void ImageWidget::setTexture(EngineObjectPtr<TextureBase> texture)
    {
        m_MaterialTexture = std::move(texture);
        if (!shouldUseSolidColor())
        {
            updateMaterialColorParams();
        }
    }
    void ImageWidget::setTextureOffset(const math::vector2& value)
    {
        m_MaterialTextureOffset = value;
        if (!shouldUseSolidColor())
        {
            updateMaterialColorParams();
        }
    }
    void ImageWidget::setTextureScale(const math::vector2& value)
    {
        m_MaterialTextureScale = value;
        if (!shouldUseSolidColor())
        {
            updateMaterialColorParams();
        }
    }

    void ImageWidget::setDepth(const float depth)
    {
        if (m_WidgetMaterial != nullptr)
        {
            static const jstringID depthParamName = JSTR("uDepth");
            m_WidgetMaterial->setParamValue<MaterialParamType::Float>(depthParamName, depth);
        }
    }

    void ImageWidget::onInitialized()
    {
        Super::onInitialized();

        recreateMaterial();
    }

    void ImageWidget::onPreRender()
    {
        Super::onPreRender();

        const WidgetContext* widgetContext = getWidgetContextPtr();
        if (widgetContext == nullptr)
        {
            return;
        }
        
        if (m_WidgetMaterial == nullptr)
        {
            return;
        }

        updateMaterialLocationParams();
        AssetsEngineSubsystem* assetsSubsystem = getEngine()->getSubsystem<AssetsEngineSubsystem>();
        widgetContext->getRenderContext().addPrimitiveToRenderList({
            assetsSubsystem->getVertexBuffer_Plane2D(), m_WidgetMaterial->getMaterial()
        });
    }

    void ImageWidget::onClear()
    {
        m_WidgetMaterial = nullptr;
        m_MaterialTexture = nullptr;

        Super::onClear();
    }

    void ImageWidget::recalculateWidetSize()
    {
        const math::box2& bounds = getWidgetBounds();
        const math::vector2 boundsSize = bounds.v1 - bounds.v0;
        if (!m_UseSolidColor)
        {
	        if (m_MaterialTexture == nullptr)
	        {
		        m_WidgetRenderSize = { 0.0f, 0.0f };
	        }
            else
            {
                const math::vector2 imageSize = math::vector2(m_MaterialTexture->getTextureBase()->getSize()) / getWidgetContextPtr()->getRenderTarget()->getSize();
                m_WidgetRenderSize = {
                    getWidgetAlignmentH() == WidgetAlignmentH::Fill ? boundsSize.x : imageSize.x,
                    getWidgetAlignmentV() == WidgetAlignmentV::Fill ? boundsSize.y : imageSize.y
                };
            }
        }
        else
        {
	        m_WidgetRenderSize = {
                getWidgetAlignmentH() == WidgetAlignmentH::Fill ? boundsSize.x : 0.0f,
                getWidgetAlignmentV() == WidgetAlignmentV::Fill ? boundsSize.y : 0.0f
            };
        }
    }

    void ImageWidget::recreateMaterial()
    {
        if (!isInitialized())
        {
            return;
        }

        m_WidgetMaterial = nullptr;

        static jstringID textureShaderName = JSTR("widgetDefault");
        static jstringID colorShaderName = JSTR("widgetSolidColor");
        AssetsEngineSubsystem* assetsSubsystem = getEngine()->getSubsystem<AssetsEngineSubsystem>();
        const EngineObjectPtr<Shader>& shader = assetsSubsystem->getEngineShader(shouldUseSolidColor() ? colorShaderName : textureShaderName);
        if (shader != nullptr)
        {
            m_WidgetMaterial = assetsSubsystem->createMaterial(shader);
        }

        updateMaterialColorParams();
    }

    void ImageWidget::updateMaterialColorParams() const
    {
        if (m_WidgetMaterial != nullptr)
        {
            if (shouldUseSolidColor())
            {
                static jstringID solidColorParam = JSTR("uColor");
                m_WidgetMaterial->setParamValue<MaterialParamType::Vec4>(solidColorParam, getColor());
            }
            else
            {
                static jstringID textureOffsetParam = JSTR("uTextureOffset");
                static jstringID textureScaleParam = JSTR("uTextureScale");
                static jstringID textureParam = JSTR("uTexture");
                m_WidgetMaterial->setParamValue<MaterialParamType::Vec2>(textureOffsetParam, getTextureOffset());
                m_WidgetMaterial->setParamValue<MaterialParamType::Vec2>(textureScaleParam, getTextureScale());
                m_WidgetMaterial->setParamValue<MaterialParamType::Texture>(textureParam, getTexture());
            }
        }
    }
    void ImageWidget::updateMaterialLocationParams() const
    {
        if (m_WidgetMaterial != nullptr)
        {
            static const jstringID locationParamName = JSTR("uLocation");
            static const jstringID sizeParamName = JSTR("uSize");
            static const jstringID depthParamName = JSTR("uDepth");

            m_WidgetMaterial->setParamValue<MaterialParamType::Vec2>(locationParamName, getWidgetRenderLocation());
            m_WidgetMaterial->setParamValue<MaterialParamType::Vec2>(sizeParamName, getWidgetRenderSize());
            m_WidgetMaterial->setParamValue<MaterialParamType::Float>(depthParamName, 0.4f);
        }
    }
}