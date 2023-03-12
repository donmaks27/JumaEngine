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

		template<MaterialParamType Type>
        bool setParamValue(const jstringID& name, const typename MaterialParamInfo<Type>::value_type& value);
		bool resetParamValue(const jstringID& name);
        template<MaterialParamType Type>
        bool getParamValue(const jstringID& name, typename MaterialParamInfo<Type>::value_type& outValue) const
        {
	        return (m_Material != nullptr) && m_Material->getParamValue(name, outValue);
        }
		template<>
		bool getParamValue<MaterialParamType::Texture>(const jstringID& name, EngineObjectPtr<TextureBase>& outValue) const
        {
	        return (m_Material != nullptr) && getTextureParamValue(name, outValue);
        }

	protected:
		
		virtual const Material* getBaseMaterial() const { return this; }
		
		virtual void onParamValueChanging(const jstringID& name) {}
		virtual bool resetParamValueInternal(const jstringID& name) { return m_Material->resetParamValue(name); }

		virtual bool getTextureParamValue(const jstringID& name, EngineObjectPtr<TextureBase>& outValue) const;

		virtual void clearAsset() final override;
		virtual void clearMaterial();

	private:

		JUTILS_CREATE_MULTICAST_DELEGATE1(OnMaterialParamEvent, const jstringID&, paramName);
		
        OnMaterialParamEvent onParamChanged;

		JumaRE::Material* m_Material = nullptr;

		jmap<jstringID, EngineObjectPtr<TextureBase>> m_ReferencedTextures;


		template<MaterialParamType Type>
        bool updateParamValue(const jstringID& name, const typename MaterialParamInfo<Type>::value_type& value) { return m_Material->setParamValue<MaterialParamInfo<Type>::uniformType>(name, value); }
        template<>
        bool updateParamValue<MaterialParamType::Texture>(const jstringID& name, const EngineObjectPtr<TextureBase>& value);
        void onTextureDestroying(EngineContextObject* object);
	};

	template<MaterialParamType Type>
	bool Material::setParamValue(const jstringID& name, const typename MaterialParamInfo<Type>::value_type& value)
	{
		const JumaRE::Shader* shader = getShader();
		const JumaRE::ShaderUniform* uniform = shader != nullptr ? shader->getUniforms().find(name) : nullptr;
		if ((uniform == nullptr) || (uniform->type != MaterialParamInfo<Type>::uniformType))
		{
			return false;
		}
		onParamValueChanging(name);
		if (!this->updateParamValue<Type>(name, value))
		{
			return false;
		}
		onParamChanged.call(name);
		return true;
	}
}
