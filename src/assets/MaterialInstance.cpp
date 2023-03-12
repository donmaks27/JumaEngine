// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/MaterialInstance.h"

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

        const JumaRE::MaterialParamsStorage& parentMaterialParams = m_ParentMaterial->getMaterial()->getMaterialParams();
        for (const auto& uniform : shader->getUniforms())
        {
	        CopyMaterialParam(uniform.value.type, material, parentMaterialParams, uniform.key);
        }

        m_ParentMaterial->onParamChanged.bind(this, &MaterialInstance::onBaseMaterialParamChanged);
        m_ParentMaterial->onDestroying.bind(this, &MaterialInstance::onBaseMaterialDestroying);
        return true;
	}

	void MaterialInstance::onParamValueChanging(const jstringID& name)
	{
		m_OverridedParams.add(name);
		Super::onParamValueChanging(name);
	}
	bool MaterialInstance::resetParamValueInternal(const jstringID& name)
	{
		m_OverridedParams.remove(name);
        return CopyMaterialParam(getMaterial(), m_ParentMaterial->getMaterial()->getMaterialParams(), name);
	}

	bool MaterialInstance::getTextureParamValue(const jstringID& name, EngineObjectPtr<TextureBase>& outValue) const
	{
        return m_OverridedParams.contains(name) ? Super::getTextureParamValue(name, outValue) : m_ParentMaterial->getParamValue<MaterialParamType::Texture>(name, outValue);
	}

	void MaterialInstance::onBaseMaterialParamChanged(const jstringID& paramName)
	{
        if (!m_OverridedParams.contains(paramName))
        {
	        if (CopyMaterialParam(getMaterial(), m_ParentMaterial->getMaterial()->getMaterialParams(), paramName))
	        {
		        onParamChanged.call(paramName);
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
