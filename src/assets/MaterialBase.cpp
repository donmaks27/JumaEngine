// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/MaterialBase.h"

#include "JumaEngine/assets/AssetsEngineSubsystem.h"
#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
	bool MaterialBase::loadMaterial(MaterialBaseCreateInfo createInfo)
	{
		JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
		JumaRE::Shader* shader = renderEngine->createShader(createInfo.shaderFiles, std::move(createInfo.shaderVertexComponents), std::move(createInfo.shaderUniforms));
		if (shader == nullptr)
		{
			JUTILS_LOG(error, JSTR("Failed to create shader"));
			return false;
		}

		JumaRE::Material* material = renderEngine->createMaterial(shader);
		if (material == nullptr)
		{
			JUTILS_LOG(error, JSTR("Failed to create material"));
			renderEngine->destroyShader(shader);
			return false;
		}

		m_Material = material;

		jset<jstringID> usedUniforms;
		const jmap<jstringID, JumaRE::ShaderUniform>& uniforms = shader->getUniforms();
		for (const auto& materialParam : createInfo.params)
		{
			if (m_MaterialParamsUniform.contains(materialParam.materialParamName) || usedUniforms.contains(materialParam.uniformName))
			{
				continue;
			}
			const JumaRE::ShaderUniform* uniform = uniforms.find(materialParam.uniformName);
			if (uniform == nullptr)
			{
				continue;
			}
			m_MaterialParamsUniform.add(materialParam.materialParamName, materialParam.uniformName);
			m_MaterialParams.add(materialParam.materialParamName);
			usedUniforms.add(materialParam.uniformName);
		}
		if (!createInfo.params.isEmpty())
		{
			m_DefaultParamValues = std::move(createInfo.defaultValues);
			for (const auto& param : m_MaterialParams)
			{
				resetParamValue(param);
			}
		}

		return true;
	}

	void MaterialBase::clearMaterial()
	{
		JumaRE::Shader* shader = getShader();

		Super::clearMaterial();

		if (shader != nullptr)
		{
			getEngine()->getRenderEngine()->destroyShader(shader);
		}
		m_DefaultParamValues = {};
	}
	
	bool MaterialBase::resetParamValueInternal(const jstringID& paramName, const jstringID& uniformName)
    {
		const JumaRE::ShaderUniform* uniform = getShader()->getUniforms().find(uniformName);
		if (uniform == nullptr)
		{
			return false;
		}
		switch (uniform->type)
		{
		case MaterialParamType::Float: return updateDefaultParamValue<MaterialParamType::Float>(paramName, uniformName);
		case MaterialParamType::Vec2: return updateDefaultParamValue<MaterialParamType::Vec2>(paramName, uniformName);
		case MaterialParamType::Vec4: return updateDefaultParamValue<MaterialParamType::Vec4>(paramName, uniformName);
		case MaterialParamType::Mat4: return updateDefaultParamValue<MaterialParamType::Mat4>(paramName, uniformName);
		case MaterialParamType::Texture: return updateDefaultParamValue<MaterialParamType::Texture>(paramName, uniformName);
		default: ;
		}
        return false;
    }
	template<>
	bool MaterialBase::getDefaultParamValue<MaterialParamType::Float>(const jstringID& paramName, float& outValue) const
	{
		const float* value = m_DefaultParamValues.values_float.find(paramName);
		if (value == nullptr)
		{
			return false;
		}
		outValue = *value;
		return true;
	}
	template<>
	bool MaterialBase::getDefaultParamValue<MaterialParamType::Vec2>(const jstringID& paramName, math::vector2& outValue) const
	{
		const math::vector2* value = m_DefaultParamValues.values_vec2.find(paramName);
		if (value == nullptr)
		{
			return false;
		}
		outValue = *value;
		return true;
	}
	template<>
	bool MaterialBase::getDefaultParamValue<MaterialParamType::Vec4>(const jstringID& paramName, math::vector4& outValue) const
	{
		const math::vector4* value = m_DefaultParamValues.values_vec4.find(paramName);
		if (value == nullptr)
		{
			return false;
		}
		outValue = *value;
		return true;
	}
	template<>
	bool MaterialBase::getDefaultParamValue<MaterialParamType::Mat4>(const jstringID& paramName, math::matrix4& outValue) const
	{
		const math::matrix4* value = m_DefaultParamValues.values_mat4.find(paramName);
		if (value == nullptr)
		{
			return false;
		}
		outValue = *value;
		return true;
	}
	template<>
	bool MaterialBase::getDefaultParamValue<MaterialParamType::Texture>(const jstringID& paramName, EngineObjectPtr<TextureBase>& outValue) const
	{
		const jstringID* textureID = m_DefaultParamValues.values_texture.find(paramName);
		if (textureID == nullptr)
		{
			return false;
		}
		EngineObjectPtr<TextureBase> texture = getEngine()->getSubsystem<AssetsEngineSubsystem>()->getAsset<TextureBase>(*textureID);
		if (texture == nullptr)
		{
			return false;
		}
		outValue = std::move(texture);
		return true;
	}
}
