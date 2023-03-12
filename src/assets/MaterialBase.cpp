// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/MaterialBase.h"

#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
	bool MaterialBase::loadMaterial(const MaterialBaseCreateInfo& createInfo)
	{
		JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
		JumaRE::Shader* shader = renderEngine->createShader(createInfo.shaderFiles, createInfo.shaderVertexComponents, createInfo.shaderUniforms);
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
	}
}
