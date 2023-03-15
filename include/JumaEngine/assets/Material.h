// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "Asset.h"

#include <JumaRE/material/Material.h>

#include "MaterialParamType.h"
#include "JumaRE/material/Shader.h"

namespace JumaEngine
{
	class MaterialBase;
	class MaterialInstance;

	struct MaterialDefaultParamValues
	{
		jmap<jstringID, float> values_float;
		jmap<jstringID, math::vector2> values_vec2;
		jmap<jstringID, math::vector4> values_vec4;
		jmap<jstringID, math::matrix4> values_mat4;
		jmap<jstringID, jstringID> values_texture;
	};

	class Material : public Asset
	{
		JUMAENGINE_CLASS_ABSTRACT(Material, Asset)

		friend MaterialBase;
		friend MaterialInstance;

	public:
		Material() : Asset(AssetType::Material) {}
		virtual ~Material() override = default;
		
		JumaRE::Material* getMaterial() const { return m_Material; }
		JumaRE::Shader* getShader() const { return m_Material != nullptr ? m_Material->getShader() : nullptr; }

		const jarray<jstringID>& getMaterialParams() const { return m_MaterialParams; }
		bool getUniformNameForParam(const jstringID& paramName, jstringID& outUniformName) const;

		template<MaterialParamType T>
		bool setParamValue(const jstringID& paramName, const typename MaterialParamInfo<T>::value_type& value);
		bool resetParamValue(const jstringID& paramName);
        template<MaterialParamType T>
        bool getParamValue(const jstringID& paramName, typename MaterialParamInfo<T>::value_type& outValue) const
        {
	        return (m_Material != nullptr) && m_Material->getParamValue(paramName, outValue);
        }
		template<>
		bool getParamValue<MaterialParamType::Texture>(const jstringID& paramName, EngineObjectPtr<TextureBase>& outValue) const
        {
	        return (m_Material != nullptr) && getTextureParamValue(paramName, outValue);
        }

		template<JumaRE::ShaderUniformType T>
		bool setUniformValue(const jstringID& uniformName, const typename JumaRE::ShaderUniformInfo<T>::value_type& value)
		{
			return (m_Material != nullptr) && m_Material->setParamValue<T>(uniformName, value);
		}
		bool resetUniformValue(const jstringID& uniformName) { return (m_Material != nullptr) && m_Material->resetParamValue(uniformName); }
		template<JumaRE::ShaderUniformType T>
		bool getUniformValue(const jstringID& uniformName, typename JumaRE::ShaderUniformInfo<T>::value_type& value) const
		{
			return (m_Material != nullptr) && m_Material->getParamValue<T>(uniformName, value);
		}

	protected:
		
		virtual const Material* getBaseMaterial() const { return this; }
		
		virtual void onParamValueChanging(const jstringID& paramName) {}
		virtual bool resetParamValueInternal(const jstringID& paramName, const jstringID& uniformName);
		virtual bool getTextureParamValue(const jstringID& paramName, EngineObjectPtr<TextureBase>& outValue) const;

		virtual void clearAsset() final override;
		virtual void clearMaterial();

	private:

		JUTILS_CREATE_MULTICAST_DELEGATE2(OnMaterialParamEvent, const jstringID&, paramName, const jstringID&, uniformName);
		
        OnMaterialParamEvent onParamChanged;

		JumaRE::Material* m_Material = nullptr;

		jarray<jstringID> m_MaterialParams;
		jmap<jstringID, jstringID> m_MaterialParamsUniform;

		jmap<jstringID, EngineObjectPtr<TextureBase>> m_ReferencedTextures;


		template<MaterialParamType T>
        bool updateParamValue(const jstringID& paramName, const jstringID& uniformName, const typename MaterialParamInfo<T>::value_type& value) { return m_Material->setParamValue<MaterialParamInfo<T>::uniformType>(uniformName, value); }
        template<>
        bool updateParamValue<MaterialParamType::Texture>(const jstringID& paramName, const jstringID& uniformName, const EngineObjectPtr<TextureBase>& value);
        void onTextureDestroying(EngineContextObject* object);
	};

	template<MaterialParamType T>
	bool Material::setParamValue(const jstringID& paramName, const typename MaterialParamInfo<T>::value_type& value)
	{
		jstringID uniformName = jstringID_NONE;
		if (!getUniformNameForParam(paramName, uniformName))
		{
			return false;
		}
		const JumaRE::Shader* shader = getShader();
		const JumaRE::ShaderUniform* uniform = shader != nullptr ? shader->getUniforms().find(uniformName) : nullptr;
		if ((uniform == nullptr) || (uniform->type != MaterialParamInfo<T>::uniformType))
		{
			return false;
		}
		onParamValueChanging(paramName);
		if (!this->updateParamValue<T>(paramName, uniformName, value))
		{
			return false;
		}
		onParamChanged.call(paramName, uniformName);
		return true;
	}
}
