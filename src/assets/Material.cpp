// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/Material.h"

#include "JumaEngine/Engine.h"
#include "JumaEngine/assets/AssetsEngineSubsystem.h"

namespace JumaEngine
{
    template<JumaRE::ShaderUniformType Type>
    void CopyMaterialParam(const jstringID& name, JumaRE::Material* material, const jstringID& inputName, const JumaRE::MaterialParamsStorage& materialParams)
    {
        typename JumaRE::ShaderUniformInfo<Type>::value_type value;
        if (materialParams.getValue<Type>(inputName, value))
        {
            material->setParamValue<Type>(name, value);
        }
    }
    inline void CopyMaterialParam(const JumaRE::ShaderUniformType type, const jstringID& name, JumaRE::Material* material, const jstringID& inputName, const JumaRE::MaterialParamsStorage& materialParams)
    {
        switch (type)
        {
        case JumaRE::ShaderUniformType::Float:   CopyMaterialParam<JumaRE::ShaderUniformType::Float>(name, material, inputName, materialParams); break;
        case JumaRE::ShaderUniformType::Vec2:    CopyMaterialParam<JumaRE::ShaderUniformType::Vec2>(name, material, inputName, materialParams); break;
        case JumaRE::ShaderUniformType::Vec4:    CopyMaterialParam<JumaRE::ShaderUniformType::Vec4>(name, material, inputName, materialParams); break;
        case JumaRE::ShaderUniformType::Mat4:    CopyMaterialParam<JumaRE::ShaderUniformType::Mat4>(name, material, inputName, materialParams); break;
        case JumaRE::ShaderUniformType::Texture: CopyMaterialParam<JumaRE::ShaderUniformType::Texture>(name, material, inputName, materialParams); break;
        default: ;
        }
    }
    inline void CopyMaterialParam(const JumaRE::ShaderUniformType type, const jstringID& name, JumaRE::Material* material, const JumaRE::MaterialParamsStorage& materialParams)
    {
        CopyMaterialParam(type, name, material, name, materialParams);
    }

    bool Material::createMaterial(Shader* shader)
    {
        if (shader == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invalid input shader"));
            return false;
        }

        Engine* engine = getEngine();
        JumaRE::Shader* shaderObject = shader->getShader();
        JumaRE::Material* materialObject = engine->getRenderEngine()->createMaterial(shaderObject);
        if (materialObject == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create material"));
            return false;
        }

        m_Material = materialObject;
        m_BaseShader = shader;

        AssetsEngineSubsystem* assetsSubsystem = engine->getSubsystem<AssetsEngineSubsystem>();
        const JumaRE::MaterialParamsStorage& internalParams = assetsSubsystem->getGlobalParams();
        const jmap<jstringID, jstringID>& shaderInternalParamNames = getBaseShader()->getEngineInternalParamNames();
        for (const auto& uniform : shaderObject->getUniforms())
        {
            const jstringID* internalParamName = shaderInternalParamNames.find(uniform.key);
            if (internalParamName != nullptr)
            {
                CopyMaterialParam(uniform.value.type, uniform.key, materialObject, *internalParamName, internalParams);
            }
        }

        if (!shaderInternalParamNames.isEmpty())
        {
            assetsSubsystem->onEngineInternalParamChanged.bind(this, &Material::onGlobalParamChanged);
        }
        return true;
    }
    bool Material::createMaterial(Material* baseMaterial)
    {
        if (baseMaterial == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invalid input material"));
            return false;
        }

        Engine* engine = getEngine();
        const JumaRE::Material* baseMaterialObject = baseMaterial->getMaterial();
        JumaRE::Shader* shaderObject = baseMaterialObject->getShader();
        JumaRE::Material* materialObject = engine->getRenderEngine()->createMaterial(shaderObject);
        if (materialObject == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create material"));
            return false;
        }

        m_Material = materialObject;
        m_BaseMaterial = baseMaterial;

        AssetsEngineSubsystem* assetsSubsystem = engine->getSubsystem<AssetsEngineSubsystem>();
        const JumaRE::MaterialParamsStorage& internalParams = assetsSubsystem->getGlobalParams();
        const jmap<jstringID, jstringID>& shaderInternalParamNames = getBaseShader()->getEngineInternalParamNames();
        for (const auto& uniform : shaderObject->getUniforms())
        {
            const jstringID* internalParamName = shaderInternalParamNames.find(uniform.key);
            if (internalParamName != nullptr)
            {
                CopyMaterialParam(uniform.value.type, uniform.key, materialObject, *internalParamName, internalParams);
            }
            else
            {
                CopyMaterialParam(uniform.value.type, uniform.key, materialObject, baseMaterialObject->getMaterialParams());
            }
        }

        if (!shaderInternalParamNames.isEmpty())
        {
            assetsSubsystem->onEngineInternalParamChanged.bind(this, &Material::onGlobalParamChanged);
        }
        m_BaseMaterial->onParamChanged.bind(this, &Material::onBaseMaterialParamChanged);
        m_BaseMaterial->onClear.bind(this, &Material::onBaseMaterialClear);
        return true;
    }
    void Material::clearMaterial()
    {
        if (m_Material != nullptr)
        {
            Engine* engine = getEngine();

            engine->getSubsystem<AssetsEngineSubsystem>()->onEngineInternalParamChanged.unbind(this, &Material::onGlobalParamChanged);
            if (m_BaseMaterial != nullptr)
            {
                m_BaseMaterial->onParamChanged.unbind(this, &Material::onBaseMaterialParamChanged);
                m_BaseMaterial->onClear.unbind(this, &Material::onBaseMaterialClear);
            }

            onClear.call(this);
            if (m_Material != nullptr)
            {
                engine->getRenderEngine()->destroyMaterial(m_Material);
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
            CopyMaterialParam(uniform->type, paramName, m_Material, m_BaseMaterial->getMaterial()->getMaterialParams());
            onParamChanged.call(this, paramName);
        }
    }

    bool Material::isGlobalMaterialParam(const jstringID& name) const
    {
        const Shader* shader = getBaseShader();
        return (shader != nullptr) && shader->getEngineInternalParamNames().contains(name);
    }
    void Material::onGlobalParamChanged(AssetsEngineSubsystem* subsystem, const jstringID& internalParamName)
    {
        for (const auto& internalParam : getBaseShader()->getEngineInternalParamNames())
        {
            if (internalParam.value != internalParamName)
            {
                continue;
            }

            const JumaRE::ShaderUniform* uniform = getShader()->getUniforms().find(internalParam.key);
            if (uniform != nullptr)
            {
                CopyMaterialParam(uniform->type, internalParam.key, m_Material, internalParamName, subsystem->getGlobalParams());
            }
            break;
        }
    }
}
