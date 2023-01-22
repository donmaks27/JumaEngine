// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/ImageWidget.h"

#include "JumaEngine/Engine.h"
#include "JumaEngine/subsystems/shaders/ShadersSubsystem.h"
#include "JumaEngine/subsystems/ui/UISubsystem.h"
#include "JumaEngine/subsystems/ui/WidgetContext.h"

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
    void ImageWidget::setTexture(JumaRE::TextureBase* texture)
    {
        m_MaterialTexture = texture;
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
            m_WidgetMaterial->setParamValue<ShaderUniformType::Float>(depthParamName, depth);
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

        const WidgetContext* widgetContext = getWidgetContext();
        if (widgetContext == nullptr)
        {
            return;
        }

        setActualSize(getMaxSize());
        if (m_WidgetMaterial == nullptr)
        {
            return;
        }

        updateMaterialLocationParams();
        const UISubsystem* uiSubsystem = getEngine()->getSubsystem<UISubsystem>();
        getEngine()->getRenderEngine()->addPrimitiveToRenderList(
            widgetContext->getRenderTarget(), uiSubsystem->getVertexBufferUI(), m_WidgetMaterial->getMaterial()
        );
    }

    void ImageWidget::onDestroying()
    {
        if (m_WidgetMaterial != nullptr)
        {
            ShadersSubsystem* shadersSubsystem = getEngine()->getSubsystem<ShadersSubsystem>();
            shadersSubsystem->destroyMaterial(m_WidgetMaterial);
        }

        Super::onDestroying();
    }

    void ImageWidget::recreateMaterial()
    {
        if (!isInitialized())
        {
            return;
        }

        ShadersSubsystem* shadersSubsystem = getEngine()->getSubsystem<ShadersSubsystem>();
        if (m_WidgetMaterial != nullptr)
        {
            shadersSubsystem->destroyMaterial(m_WidgetMaterial);
            m_WidgetMaterial = nullptr;
        }

        static jstringID textureShaderName = JSTR("widgetDefault");
        static jstringID colorShaderName = JSTR("widgetSolidColor");
        Shader* shader = shadersSubsystem->getEngineShader(shouldUseSolidColor() ? colorShaderName : textureShaderName);
        if (shader != nullptr)
        {
            m_WidgetMaterial = shadersSubsystem->createMaterial(shader);

            /*static const jstringID offsetParamName = JSTR("uOffset");
            const float offsetY = getEngine()->getRenderEngine()->getRenderAPI() == JumaRE::RenderAPI::Vulkan ? 1.0f : -1.0f;
            m_WidgetMaterial->setParamValue<ShaderUniformType::Vec2>(offsetParamName, { 1.0f, offsetY });*/
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
                m_WidgetMaterial->setParamValue<ShaderUniformType::Vec4>(solidColorParam, getColor());
            }
            else
            {
                static jstringID textureParam = JSTR("uTexture");
                m_WidgetMaterial->setParamValue<ShaderUniformType::Texture>(textureParam, getTexture());
            }
        }
    }
    void ImageWidget::updateMaterialLocationParams() const
    {
        if (m_WidgetMaterial != nullptr)
        {
            static const jstringID locationParamName = JSTR("uLocation");
            static const jstringID sizeParamName = JSTR("uSize");
            //static const jstringID depthParamName = JSTR("uDepth");

            m_WidgetMaterial->setParamValue<ShaderUniformType::Vec2>(locationParamName, getLocation());
            m_WidgetMaterial->setParamValue<ShaderUniformType::Vec2>(sizeParamName, getSize());
            //m_WidgetMaterial->setParamValue<ShaderUniformType::Float>(depthParamName, 0.0f);
        }
    }
}
