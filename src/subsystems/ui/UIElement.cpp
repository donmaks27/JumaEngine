// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/UIElement.h"

#include "JumaEngine/Engine.h"
#include "JumaEngine/subsystems/shaders/ShadersSubsystem.h"

namespace JumaEngine
{
    UIElement::~UIElement()
    {
        clearWidget();
    }

    void UIElement::setMaterial(Material* material)
    {
        clearWidget();

        ShadersSubsystem* shadersSubsystem = getEngine()->getSubsystem<ShadersSubsystem>();
        if (shadersSubsystem != nullptr)
        {
            m_Material = shadersSubsystem->createMaterial(material);
        }
    }

    void UIElement::updateMaterial()
    {
        static const jstringID locationParamName = JSTR("uLocation");
        static const jstringID offsetParamName = JSTR("uOffset");
        static const jstringID sizeParamName = JSTR("uSize");
        static const jstringID depthParamName = JSTR("uDepth");

        if (m_Material != nullptr)
        {
            m_Material->setParamValue<ShaderUniformType::Vec2>(locationParamName, m_Location);
            m_Material->setParamValue<ShaderUniformType::Vec2>(offsetParamName, m_Offset);
            m_Material->setParamValue<ShaderUniformType::Vec2>(sizeParamName, m_Size);
            m_Material->setParamValue<ShaderUniformType::Float>(depthParamName, m_Depth);
        }
    }

    void UIElement::clearWidget()
    {
        if (m_Material != nullptr)
        {
            ShadersSubsystem* shadersSubsystem = getEngine()->getSubsystem<ShadersSubsystem>();
            if (shadersSubsystem != nullptr)
            {
                shadersSubsystem->destroyMaterial(m_Material);
            }
            m_Material = nullptr;
        }
    }
}
