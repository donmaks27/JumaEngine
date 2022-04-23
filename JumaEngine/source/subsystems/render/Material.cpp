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

        markAsInitialized();
        return true;
    }

    Material_RenderAPIObject* Material::createRenderAPIObjectInternal()
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
            case ShaderUniformType::RenderTarget:
                m_UniformValues_RenderTarget.add(uniformName, nullptr);
                break;

            default: ;
            }
        }
    }
    void Material::clearUniformValues()
    {
        for (const auto& textureValue : m_UniformValues_Texture)
        {
            if (textureValue.value != nullptr)
            {
                textureValue.value->onClear.unbind(this, &Material::onTextureCleared);
            }
        }
        for (const auto& renderTargetValue : m_UniformValues_RenderTarget)
        {
            if (renderTargetValue.value != nullptr)
            {
                renderTargetValue.value->onClear.unbind(this, &Material::onRenderTargetCleared);
            }
        }

        m_UniformValues_Mat4.clear();
        m_UniformValues_Texture.clear();
        m_UniformValues_RenderTarget.clear();
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
        case ShaderUniformType::RenderTarget: return m_UniformValues_RenderTarget.contains(paramName);

        default: ;
        }
        return false;
    }
    
    bool Material::isUniformTypeCorrect(const jstringID& paramName, const ShaderUniformType type) const
    {
        const ShaderUniform* uniform = m_BaseShader->getUniforms().find(paramName);
        return (uniform != nullptr) && (uniform->type == type);
    }

    void Material::resetParamValue(const jstringID& paramName)
    {
        const ShaderUniformType type = getParamType(paramName);
        switch (type)
        {
        case ShaderUniformType::Mat4:
            {
                if (isMaterialInstance())
                {
                    m_UniformValues_Mat4.remove(paramName);
                }
                else
                {
                    math::matrix4& value = m_UniformValues_Mat4[paramName];
                    if (!math::isMatricesEqual(value, math::matrix4(1)))
                    {
                        value = math::matrix4(1);
                    }
                }
            }
            break;

        case ShaderUniformType::Texture:
            {
                Texture** valuePtr = m_UniformValues_Texture.find(paramName);
                if (valuePtr != nullptr)
                {
                    Texture* value = *valuePtr;
                    if (value != nullptr)
                    {
                        value->onClear.unbind(this, &Material::onTextureCleared);
                    }
                    if (isMaterialInstance())
                    {
                        m_UniformValues_Texture.remove(paramName);
                    }
                    else if (value != nullptr)
                    {
                        *valuePtr = nullptr;
                    }
                }
            }
            break;

        case ShaderUniformType::RenderTarget:
            {
                RenderTarget** valuePtr = m_UniformValues_RenderTarget.find(paramName);
                if (valuePtr != nullptr)
                {
                    RenderTarget* value = *valuePtr;
                    if (value != nullptr)
                    {
                        value->onClear.unbind(this, &Material::onRenderTargetCleared);
                    }
                    if (isMaterialInstance())
                    {
                        m_UniformValues_RenderTarget.remove(paramName);
                    }
                    else if (value != nullptr)
                    {
                        *valuePtr = nullptr;
                    }
                }
            }
            break;

        default: ;
        }
    }

    bool Material::setParamValueInternal_Texture(const jstringID& paramName, const texture_value_type& value)
    {
        const texture_value_type* prevTexturePtr = m_UniformValues_Texture.find(paramName);
        const texture_value_type prevTexture = prevTexturePtr != nullptr ? *prevTexturePtr : nullptr;
        const texture_value_type newValue = (value != nullptr) && value->isValid() ? value : nullptr;
        if (prevTexture == newValue)
        {
            return false;
        }

        if (prevTexture != nullptr)
        {
            prevTexture->onClear.unbind(this, &Material::onTextureCleared);
        }
        if (newValue != nullptr)
        {
            newValue->onClear.bind(this, &Material::onTextureCleared);
        }

        m_UniformValues_Texture[paramName] = newValue;
        return true;
    }
    void Material::onTextureCleared(Texture* texture)
    {
        jarray<jstringID> paramNames;
        for (const auto& textureParam : m_UniformValues_Texture)
        {
            if (textureParam.value == texture)
            {
                paramNames.add(textureParam.key);
            }
        }
        for (const auto& paramName : paramNames)
        {
            resetParamValue(paramName);
        }
    }

    bool Material::setParamValueInternal_RenderTarget(const jstringID& paramName, const render_target_value_type& value)
    {
        const render_target_value_type* prevRenderTargerPtr = m_UniformValues_RenderTarget.find(paramName);
        const render_target_value_type prevRenderTarger = prevRenderTargerPtr != nullptr ? *prevRenderTargerPtr : nullptr;
        const render_target_value_type newValue = (value != nullptr) && value->isValid() ? value : nullptr;
        if (prevRenderTarger == newValue)
        {
            return false;
        }

        if (prevRenderTarger != nullptr)
        {
            prevRenderTarger->onClear.unbind(this, &Material::onRenderTargetCleared);
        }
        if (newValue != nullptr)
        {
            newValue->onClear.bind(this, &Material::onRenderTargetCleared);
        }

        m_UniformValues_RenderTarget[paramName] = newValue;
        return true;
    }
    void Material::onRenderTargetCleared(RenderTarget* texture)
    {
        jarray<jstringID> paramNames;
        for (const auto& textureParam : m_UniformValues_RenderTarget)
        {
            if (textureParam.value == texture)
            {
                paramNames.add(textureParam.key);
            }
        }
        for (const auto& paramName : paramNames)
        {
            resetParamValue(paramName);
        }
    }
}
