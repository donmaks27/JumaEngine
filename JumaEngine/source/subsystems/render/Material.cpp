// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Material.h"

#include "RenderSubsystem.h"
#include "RenderTarget.h"
#include "engine/Engine.h"
#include "Shader.h"
#include "Texture.h"
#include "jutils/math/math_matrix.h"

namespace JumaEngine
{
    Material::~Material()
    {
        clearData();
    }

    Material_RenderAPIObject* Material::createRenderAPIObjectInternal()
    {
        return getOwnerEngine()->getRenderSubsystem()->createMaterialObject();
    }

    bool Material::init(Shader* shader)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Material already initialized"));
            return false;
        }
        if ((shader == nullptr) || !shader->isValid())
        {
            JUMA_LOG(warning, JSTR("Invalid shader"));
            return false;
        }

        m_BaseMaterial = nullptr;
        m_BaseShader = shader;
        m_BaseShader->onClear.bind(this, &Material::onBaseShaderClear);
        for (const auto& uniform : m_BaseShader->getUniforms())
        {
            m_MaterialParams.setDefaultValue(uniform.value.type, uniform.key);
        }

        markAsInitialized();
        return true;
    }
    bool Material::init(Material* baseMaterial)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Material already initialized"));
            return false;
        }
        if ((baseMaterial == nullptr) || !baseMaterial->isValid())
        {
            JUMA_LOG(warning, JSTR("Invalid base material"));
            return false;
        }

        m_BaseMaterial = baseMaterial;
        m_BaseShader = m_BaseMaterial->getShader();
        m_BaseMaterial->onClear.bind(this, &Material::onBaseMaterialClear);

        markAsInitialized();
        return true;
    }

    void Material::clearData()
    {
        if (isMaterialInstance())
        {
            m_BaseMaterial->onClear.unbind(this, &Material::onBaseMaterialClear);
        }
        else
        {
            m_BaseShader->onClear.unbind(this, &Material::onBaseShaderClear);
        }
        onClear.call(this);

        clearRenderAPIObject();

        m_MaterialParams.clear();
        m_BaseMaterial = nullptr;
        m_BaseShader = nullptr;
    }

    bool Material::render(VertexBuffer* vertexBuffer, const RenderOptions* renderOptions)
    {
        Material_RenderAPIObject* renderObject = getRenderAPIObject();
        if (renderObject == nullptr)
        {
            return false;
        }
        return renderObject->render(vertexBuffer, renderOptions);
    }

    ShaderUniformType Material::getParamType(const jstringID& paramName) const
    {
        if (isValid())
        {
            const ShaderUniform* uniform = getShader()->getUniforms().find(paramName);
            if (uniform != nullptr)
            {
                return uniform->type;
            }
        }
        return ShaderUniformType::None;
    }
    bool Material::isOverrideParam(const jstringID& paramName) const
    {
        if (!isValid())
        {
            return false;
        }
        if (!isMaterialInstance())
        {
            return true;
        }

        const ShaderUniform* uniform = m_BaseShader->getUniforms().find(paramName);
        if (uniform == nullptr)
        {
            return false;
        }
        return m_MaterialParams.contains(uniform->type, paramName);
    }
    
    bool Material::isUniformTypeCorrect(const jstringID& paramName, const ShaderUniformType type) const
    {
        const ShaderUniform* uniform = m_BaseShader->getUniforms().find(paramName);
        return (uniform != nullptr) && (uniform->type == type);
    }
    void Material::resetParamValue(const jstringID& paramName)
    {
        const ShaderUniformType type = getParamType(paramName);
        if (isMaterialInstance())
        {
            m_MaterialParams.removeValue(type, paramName);
        }
        else
        {
            m_MaterialParams.setDefaultValue(type, paramName);
        }
    }
}
