// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "Material.h"

namespace JumaEngine
{
	struct MaterialBaseCreateInfo
	{
		jmap<JumaRE::ShaderStageFlags, jstring> shaderFiles;
		jset<jstringID> shaderVertexComponents;
		jmap<jstringID, JumaRE::ShaderUniform> shaderUniforms;
	};

	class MaterialBase final : public Material
	{
		JUMAENGINE_CLASS(MaterialBase, Material)

		friend AssetsEngineSubsystem;

	public:
		MaterialBase() = default;
		virtual ~MaterialBase() override = default;

	protected:
		
		virtual void clearMaterial() override;

	private:

		bool loadMaterial(const MaterialBaseCreateInfo& createInfo);
	};
}
