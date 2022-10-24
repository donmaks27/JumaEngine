// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../../../include/JumaEngine/subsystems/shaders/Material.h"

#include "../../../include/JumaEngine/Engine.h"
#include "../../../include/JumaEngine/subsystems/shaders/Shader.h"

namespace JumaEngine
{
    template<JumaRE::ShaderUniformType Type>
    void CopyMaterialParam(const jstringID& name, JumaRE::Material* material, const JumaRE::Material* baseMaterial)
    {
        typename JumaRE::ShaderUniformInfo<Type>::value_type value;
        if (baseMaterial->getParamValue<Type>(name, value))
        {
            material->setParamValue<Type>(name, value);
        }
    }
    inline void CopyMaterialParam(const JumaRE::ShaderUniformType type, const jstringID& name, JumaRE::Material* material, const JumaRE::Material* baseMaterial)
    {
        switch (type)
        {
        case JumaRE::ShaderUniformType::Float:   CopyMaterialParam<JumaRE::ShaderUniformType::Float>(name, material, baseMaterial); break;
        case JumaRE::ShaderUniformType::Vec2:    CopyMaterialParam<JumaRE::ShaderUniformType::Vec2>(name, material, baseMaterial); break;
        case JumaRE::ShaderUniformType::Vec4:    CopyMaterialParam<JumaRE::ShaderUniformType::Vec4>(name, material, baseMaterial); break;
        case JumaRE::ShaderUniformType::Mat4:    CopyMaterialParam<JumaRE::ShaderUniformType::Mat4>(name, material, baseMaterial); break;
        case JumaRE::ShaderUniformType::Texture: CopyMaterialParam<JumaRE::ShaderUniformType::Texture>(name, material, baseMaterial); break;
        default: ;
        }
    }

    bool Material::createMaterial(Shader* shader)
    {
        if (shader == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invalid input shader"));
            return false;
        }

        JumaRE::Material* material = getEngine()->getRenderEngine()->createMaterial(shader->getShader());
        if (material == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create material"));
            return false;
        }

        m_Material = material;
        m_BaseShader = shader;
        return true;
    }
    bool Material::createMaterial(Material* baseMaterial)
    {
        if (baseMaterial == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invalid input material"));
            return false;
        }
        
        const JumaRE::Material* baseMaterialObject = baseMaterial->getMaterial();
        JumaRE::Shader* shaderObject = baseMaterialObject->getShader();
        JumaRE::Material* materialObject = getEngine()->getRenderEngine()->createMaterial(shaderObject);
        if (materialObject == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create material"));
            return false;
        }
        for (const auto& uniform : shaderObject->getUniforms())
        {
            CopyMaterialParam(uniform.value.type, uniform.key, materialObject, baseMaterialObject);
        }

        m_Material = materialObject;
        m_BaseMaterial = baseMaterial;
        m_BaseMaterial->onParamChanged.bind(this, &Material::onBaseMaterialParamChanged);
        m_BaseMaterial->onClear.bind(this, &Material::onBaseMaterialClear);
        return true;
    }
    void Material::clearMaterial()
    {
        if (m_Material != nullptr)
        {
            if (m_BaseMaterial != nullptr)
            {
                m_BaseMaterial->onParamChanged.unbind(this, &Material::onBaseMaterialParamChanged);
                m_BaseMaterial->onClear.unbind(this, &Material::onBaseMaterialClear);
            }

            onClear.call(this);
            if (m_Material != nullptr)
            {
                getEngine()->getRenderEngine()->destroyMaterial(m_Material);
            }

            m_Material = nullptr;
            m_BaseShader = nullptr;
            m_BaseMaterial = nullptr;
        }
    }

    void Material::onBaseMaterialParamChanged(Material* baseMaterial, const jstringID& paramName)
    {
        const JumaRE::ShaderUniform* uniform = getShader()->getUniforms().find(paramName);
        if (uniform != nullptr)
        {
            CopyMaterialParam(uniform->type, paramName, m_Material, m_BaseMaterial->getMaterial());
        }
    }
}
