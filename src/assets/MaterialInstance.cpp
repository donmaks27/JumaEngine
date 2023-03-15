// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/MaterialInstance.h"

#include "JumaEngine/assets/AssetsEngineSubsystem.h"
#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
	template<JumaRE::ShaderUniformType Type>
    bool CopyMaterialParamTemplate(JumaRE::Material* material, const jstringID& writeName, const JumaRE::MaterialParamsStorage& materialParams, const jstringID& readName)
    {
        typename JumaRE::ShaderUniformInfo<Type>::value_type value;
        if (materialParams.getValue<Type>(readName, value))
        {
            return material->setParamValue<Type>(writeName, value);
        }
        return false;
    }
	inline bool CopyMaterialParam(const JumaRE::ShaderUniformType type, JumaRE::Material* material, const jstringID& writeName, const JumaRE::MaterialParamsStorage& materialParams, const jstringID& readName)
    {
        switch (type)
        {
        case JumaRE::ShaderUniformType::Float:
            return CopyMaterialParamTemplate<JumaRE::ShaderUniformType::Float>(material, writeName, materialParams, readName);
        case JumaRE::ShaderUniformType::Vec2:
            return CopyMaterialParamTemplate<JumaRE::ShaderUniformType::Vec2>(material, writeName, materialParams, readName);
        case JumaRE::ShaderUniformType::Vec4:
            return CopyMaterialParamTemplate<JumaRE::ShaderUniformType::Vec4>(material, writeName, materialParams, readName);
        case JumaRE::ShaderUniformType::Mat4:
            return CopyMaterialParamTemplate<JumaRE::ShaderUniformType::Mat4>(material, writeName, materialParams, readName);
        case JumaRE::ShaderUniformType::Texture:
            return CopyMaterialParamTemplate<JumaRE::ShaderUniformType::Texture>(material, writeName, materialParams, readName);
        default: ;
        }
        return false;
    }
    inline bool CopyMaterialParam(JumaRE::Material* material, const jstringID& writeName, const JumaRE::MaterialParamsStorage& materialParams, const jstringID& readName)
    {
        const JumaRE::ShaderUniform* uniform = material->getShader()->getUniforms().find(writeName);
        if (uniform != nullptr)
        {
            return CopyMaterialParam(uniform->type, material, writeName, materialParams, readName);
        }
        return false;
    }
    inline bool CopyMaterialParam(const JumaRE::ShaderUniformType type, JumaRE::Material* material, const JumaRE::MaterialParamsStorage& materialParams, const jstringID& name)
    {
        return CopyMaterialParam(type, material, name, materialParams, name);
    }
    inline bool CopyMaterialParam(JumaRE::Material* material, const JumaRE::MaterialParamsStorage& materialParams, const jstringID& name)
    {
        const JumaRE::ShaderUniform* uniform = material->getShader()->getUniforms().find(name);
        if (uniform != nullptr)
        {
            return CopyMaterialParam(uniform->type, material, materialParams, name);
        }
        return false;
    }
    
	bool MaterialInstance::createMaterial(const MaterialInstanceCreateInfo& createInfo)
	{
        if (createInfo.parentMaterial == nullptr)
        {
	        JUTILS_LOG(error, JSTR("Invalid parent material"));
            return false;
        }

        JumaRE::Shader* shader = createInfo.parentMaterial->getShader();
        JumaRE::Material* material = getEngine()->getRenderEngine()->createMaterial(shader);
        if (material == nullptr)
        {
	        JUTILS_LOG(error, JSTR("Failed to create material"));
            return false;
        }

        m_Material = material;
        m_ParentMaterial = createInfo.parentMaterial;
        m_MaterialParamsUniform = m_ParentMaterial->m_MaterialParamsUniform;
        m_MaterialParams = m_ParentMaterial->m_MaterialParams;

        m_ParentMaterial->onParamChanged.bind(this, &MaterialInstance::onBaseMaterialParamChanged);
        m_ParentMaterial->onDestroying.bind(this, &MaterialInstance::onBaseMaterialDestroying);

        const jmap<jstringID, JumaRE::ShaderUniform>& uniforms = shader->getUniforms();
        const JumaRE::MaterialParamsStorage& parentMaterialParams = m_ParentMaterial->getMaterial()->getMaterialParams();
        for (const auto& materialParam : m_MaterialParams)
        {
            jstringID uniformName;
            getUniformNameForParam(materialParam, uniformName);
            const JumaRE::ShaderUniform* uniform = uniforms.find(uniformName);
            if (uniform == nullptr)
            {
                continue;
            }
            switch (uniform->type)
            {
            case JumaRE::ShaderUniformType::Float:
                {
                    const auto* value = createInfo.overridedParams.values_float.find(materialParam);
                    if (value != nullptr)
                    {
                        setParamValue<MaterialParamType::Float>(materialParam, *value);
                        continue;
                    }
                }
                break;
            case JumaRE::ShaderUniformType::Vec2:
                {
                    const auto* value = createInfo.overridedParams.values_vec2.find(materialParam);
                    if (value != nullptr)
                    {
                        setParamValue<MaterialParamType::Vec2>(materialParam, *value);
                        continue;
                    }
                }
                break;
            case JumaRE::ShaderUniformType::Vec4:
                {
                    const auto* value = createInfo.overridedParams.values_vec4.find(materialParam);
                    if (value != nullptr)
                    {
                        setParamValue<MaterialParamType::Vec4>(materialParam, *value);
                        continue;
                    }
                }
                break;
            case JumaRE::ShaderUniformType::Mat4:
                {
                    const auto* value = createInfo.overridedParams.values_mat4.find(materialParam);
                    if (value != nullptr)
                    {
                        setParamValue<MaterialParamType::Mat4>(materialParam, *value);
                        continue;
                    }
                }
                break;
            case JumaRE::ShaderUniformType::Texture:
                {
                    const auto* value = createInfo.overridedParams.values_texture.find(materialParam);
                    if (value != nullptr)
                    {
                        setParamValue<MaterialParamType::Texture>(
                            materialParam, getEngine()->getSubsystem<AssetsEngineSubsystem>()->getTextureAsset(*value)
                        );
                        continue;
                    }
                }
                break;
            default: ;
            }
            CopyMaterialParam(uniform->type, material, parentMaterialParams, uniformName);
        }
        return true;
	}

	void MaterialInstance::onParamValueChanging(const jstringID& paramName)
	{
		m_OverridedParams.add(paramName);
		Super::onParamValueChanging(paramName);
	}
	bool MaterialInstance::resetParamValueInternal(const jstringID& paramName, const jstringID& uniformName)
	{
		m_OverridedParams.remove(paramName);
        Super::resetParamValueInternal(paramName, uniformName);
        return CopyMaterialParam(getMaterial(), m_ParentMaterial->getMaterial()->getMaterialParams(), uniformName);
	}
	bool MaterialInstance::getTextureParamValue(const jstringID& paramName, EngineObjectPtr<TextureBase>& outValue) const
	{
        return m_OverridedParams.contains(paramName) ? Super::getTextureParamValue(paramName, outValue) : m_ParentMaterial->getParamValue<MaterialParamType::Texture>(paramName, outValue);
	}

	void MaterialInstance::onBaseMaterialParamChanged(const jstringID& paramName, const jstringID& uniformName)
	{
        if (!m_OverridedParams.contains(paramName))
        {
	        if (CopyMaterialParam(getMaterial(), m_ParentMaterial->getMaterial()->getMaterialParams(), uniformName))
	        {
		        onParamChanged.call(paramName, uniformName);
	        }
        }
	}
    void MaterialInstance::onBaseMaterialDestroying(EngineContextObject* object)
	{
        clearAsset();
	}

	void MaterialInstance::clearMaterial()
	{
        m_ParentMaterial->onParamChanged.unbind(this, &MaterialInstance::onBaseMaterialParamChanged);
        m_ParentMaterial->onDestroying.unbind(this, &MaterialInstance::onBaseMaterialDestroying);
        m_ParentMaterial = nullptr;

		Super::clearMaterial();

        m_OverridedParams.clear();
	}
}
