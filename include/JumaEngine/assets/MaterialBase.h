// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "Material.h"

#include <JumaRE/RenderEngine.h>

namespace JumaEngine
{
	struct MaterialParamCreateInfo
	{
		jstringID materialParamName;
		jstringID uniformName;
	};
	struct MaterialBaseCreateInfo
	{
		JumaRE::RenderEngine::ShaderCreateInfo shaderInfo;

		jarray<MaterialParamCreateInfo> params;
		MaterialDefaultParamValues defaultValues;
	};

	class MaterialBase final : public Material
	{
		JUMAENGINE_CLASS(MaterialBase, Material)

		friend AssetsEngineSubsystem;

	public:
		MaterialBase() = default;
		virtual ~MaterialBase() override = default;

	protected:
		
		virtual bool resetParamValueInternal(const jstringID& paramName, const jstringID& uniformName) override;

		virtual void clearMaterial() override;

	private:

		MaterialDefaultParamValues m_DefaultParamValues;


		bool loadMaterial(const MaterialBaseCreateInfo& createInfo);

		template<MaterialParamType T>
		bool getDefaultParamValue(const jstringID& paramName, typename MaterialParamInfo<T>::value_type& outValue) const { return false; }
		template<>
		bool getDefaultParamValue<MaterialParamType::Float>(const jstringID& paramName, float& outValue) const;
		template<>
		bool getDefaultParamValue<MaterialParamType::Vec2>(const jstringID& paramName, math::vector2& outValue) const;
		template<>
		bool getDefaultParamValue<MaterialParamType::Vec4>(const jstringID& paramName, math::vector4& outValue) const;
		template<>
		bool getDefaultParamValue<MaterialParamType::Mat4>(const jstringID& paramName, math::matrix4& outValue) const;
		template<>
		bool getDefaultParamValue<MaterialParamType::Texture>(const jstringID& paramName, EngineObjectPtr<TextureBase>& outValue) const;

		template<MaterialParamType T>
		bool updateDefaultParamValue(const jstringID& paramName, const jstringID& uniformName)
		{
			typename MaterialParamInfo<T>::value_type value;
			if (!this->getDefaultParamValue<T>(paramName, value))
			{
				return m_Material->resetParamValue(uniformName);
			}
			return this->updateParamValue<T>(paramName, uniformName, value);
		}
	};
}
