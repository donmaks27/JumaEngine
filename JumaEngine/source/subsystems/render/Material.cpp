// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Material.h"
#include "RenderSubsystem.h"
#include "Shader.h"
#include "engine/Engine.h"
#include "jutils/math/math_matrix.h"

namespace JumaEngine
{
    Material::~Material()
    {
        clear();
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

        m_Initialized = true;
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

        m_Initialized = true;
        return true;
    }
    
    void Material::clear()
    {
        if (isValid())
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

            clearRenderObject();

            clearUniformValues();

            m_BaseMaterial = nullptr;
            m_BaseShader = nullptr;
            m_Initialized = false;
        }
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
            case ShaderUniformType::Image: break;

            default: ;
            }
        }
    }
    void Material::clearUniformValues()
    {
        m_UniformValues_Mat4.clear();
    }

    bool Material::createRenderObject()
    {
        if (!isValid())
        {
            JUMA_LOG(warning, JSTR("Shader not initialized"));
            return false;
        }

        RenderSubsystem* renderSubsystem = getOwnerEngine()->getRenderSubsystem();
        m_RenderObject = renderSubsystem->createMaterialObject();
        if (!m_RenderObject->init(this))
        {
            delete m_RenderObject;
            m_RenderObject = nullptr;
            return false;
        }
        return true;
    }
    void Material::clearRenderObject()
    {
        if (m_RenderObject != nullptr)
        {
            delete m_RenderObject;
            m_RenderObject = nullptr;
        }
    }

    bool Material::render(VertexBuffer* vertexBuffer, const RenderOptions* renderOptions)
    {
        if (m_RenderObject == nullptr)
        {
            return false;
        }
        return m_RenderObject->render(vertexBuffer, renderOptions);
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
        if (m_RenderObject != nullptr)
        {
            m_RenderObject->onMaterialParamChanged(paramName);
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
        bool success = false;
        switch (getParamType(paramName))
        {
        case ShaderUniformType::Mat4:
            {
                if (isMaterialInstance())
                {
                    success = m_UniformValues_Mat4.remove(paramName);
                }
                else
                {
                    math::matrix4& value = m_UniformValues_Mat4[paramName];
                    if (!math::isMatricesEqual(value, math::matrix4(1)))
                    {
                        value = math::matrix4(1);
                        success = true;
                    }
                }
            }
            break;

        default: ;
        }
        if (success)
        {
            notifyMaterialParamChanged(paramName);
        }
    }
}
