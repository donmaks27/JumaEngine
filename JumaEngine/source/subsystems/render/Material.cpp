// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Material.h"

#include "RenderSubsystem.h"
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

        m_BaseShader = shader;
        createUniformValues(shader);
        m_BaseShader->onClear.bind(this, &Material::onBaseShaderClear);

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
        m_BaseMaterial->onParamChanged.bind(this, &Material::onBaseMaterialParamChanged);

        markAsInitialized();
        return true;
    }

    MaterialRenderAPIObject* Material::createRenderAPIObjectInternal()
    {
        return getOwnerEngine()->getRenderSubsystem()->createMaterialObject();
    }

    void Material::clearData()
    {
        if (!isMaterialInstance())
        {
            m_BaseShader->onClear.unbind(this, &Material::onBaseShaderClear);
        }
        else
        {
            m_BaseMaterial->onParamChanged.unbind(this, &Material::onBaseMaterialParamChanged);
            m_BaseMaterial->onClear.unbind(this, &Material::onBaseMaterialClear);
        }
        onClear.call(this);

        clearRenderAPIObject();

        clearUniformValues();

        m_BaseMaterial = nullptr;
        m_BaseShader = nullptr;
    }

    void Material::createUniformValues(Shader* shader)
    {
        for (const auto& uniformNameAndType : shader->getUniforms())
        {
            const jstringID& uniformName = uniformNameAndType.key;
            const ShaderUniform& uniformType = uniformNameAndType.value;
            switch (uniformType.type)
            {
            case ShaderUniformType::Mat4: 
                m_UniformValues_Mat4.add(uniformName, math::matrix4(1));
                break;
            case ShaderUniformType::Texture: 
                m_UniformValues_Texture.add(uniformName, nullptr);
                break;

            default: ;
            }
        }
    }
    void Material::clearUniformValues()
    {
        m_UniformValues_Mat4.clear();
    }

    bool Material::render(VertexBuffer* vertexBuffer, const RenderOptions* renderOptions)
    {
        MaterialRenderAPIObject* renderObject = getRenderAPIObject();
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
            const ShaderUniform* uniform = m_BaseShader->getUniforms().find(paramName);
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

        switch (uniform->type)
        {
        case ShaderUniformType::Mat4: return m_UniformValues_Mat4.contains(paramName);
        case ShaderUniformType::Texture: return m_UniformValues_Texture.contains(paramName);

        default: ;
        }
        return false;
    }
    
    void Material::onBaseMaterialParamChanged(Material* material, const jstringID& paramName)
    {
        if (!isOverrideParam(paramName))
        {
            notifyMaterialParamChanged(paramName);
        }
    }
    void Material::notifyMaterialParamChanged(const jstringID& paramName)
    {
        MaterialRenderAPIObject* renderObject = getRenderAPIObject();
        if (renderObject != nullptr)
        {
            renderObject->onMaterialParamChanged(paramName);
        }
        onParamChanged.call(this, paramName);
    }

    bool Material::isUniformTypeCorrect(const jstringID& paramName, const ShaderUniformType type) const
    {
        const ShaderUniform* uniform = m_BaseShader->getUniforms().find(paramName);
        return (uniform != nullptr) && (uniform->type == type);
    }

    void Material::resetParamValue(const jstringID& paramName)
    {
        bool paramChanged = false;
        const ShaderUniformType type = getParamType(paramName);
        if (isMaterialInstance())
        {
            switch (type)
            {
            case ShaderUniformType::Mat4:
                paramChanged = m_UniformValues_Mat4.remove(paramName);
                break;
            case ShaderUniformType::Texture:
                paramChanged = m_UniformValues_Texture.remove(paramName);
                break;

            default: ;
            }
        }
        else
        {
            switch (type)
            {
            case ShaderUniformType::Mat4:
                {
                    math::matrix4& value = m_UniformValues_Mat4[paramName];
                    if (!math::isMatricesEqual(value, math::matrix4(1)))
                    {
                        value = math::matrix4(1);
                        paramChanged = true;
                    }
                }
                break;
            case ShaderUniformType::Texture:
                {
                    Texture*& value = m_UniformValues_Texture[paramName];
                    if (value != nullptr)
                    {
                        value = nullptr;
                        paramChanged = true;
                    }
                }
                break;

            default: ;
            }
        }
        if (paramChanged)
        {
            notifyMaterialParamChanged(paramName);
        }
    }
}
