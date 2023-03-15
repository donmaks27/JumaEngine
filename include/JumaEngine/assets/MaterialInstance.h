// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "Material.h"

namespace JumaEngine
{
	struct MaterialInstanceCreateInfo
	{
		EngineObjectPtr<Material> parentMaterial = nullptr;
		MaterialDefaultParamValues overridedParams;
	};

	class MaterialInstance final : public Material
	{
		JUMAENGINE_CLASS(MaterialInstance, Material)

		friend AssetsEngineSubsystem;

	public:
		MaterialInstance() = default;
		virtual ~MaterialInstance() override = default;

	protected:

		virtual const Material* getBaseMaterial() const override { return m_ParentMaterial != nullptr ? m_ParentMaterial->getBaseMaterial() : nullptr; }

		virtual void onParamValueChanging(const jstringID& name) override;
		virtual bool resetParamValueInternal(const jstringID& name, const jstringID& uniformName) override;
		virtual bool getTextureParamValue(const jstringID& name, EngineObjectPtr<TextureBase>& outValue) const override;

		virtual void clearMaterial() override;
		
	private:

		EngineObjectPtr<Material> m_ParentMaterial = nullptr;

		jset<jstringID> m_OverridedParams;


		bool createMaterial(const MaterialInstanceCreateInfo& createInfo);

		void onBaseMaterialParamChanged(const jstringID& paramName, const jstringID& uniformName);
		void onBaseMaterialDestroying(EngineContextObject* object);
	};
}
