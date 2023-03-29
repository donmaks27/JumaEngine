// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/MaterialBase.h"

#include "JumaEngine/assets/AssetsEngineSubsystem.h"
#include "JumaEngine/engine/AsyncEngineSubsystem.h"
#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
	bool MaterialBase::loadMaterial(const MaterialBaseCreateInfo& createInfo)
	{
		JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();

		std::atomic_bool flag = false;
		JumaRE::Shader* shader = nullptr;
		JumaRE::Material* material = nullptr;
		const bool result = renderEngine->createShaderAsync(createInfo.shaderInfo, [renderEngine, &flag, &shader, &material](JumaRE::Shader* createdShader)
		{
		    shader = createdShader;
			material = renderEngine->createMaterial(shader);
			flag = true;
		});
		if (result)
		{
		    while (!flag);
		}
		//JumaRE::Shader* shader = renderEngine->createShader(createInfo.shaderInfo);
		if (shader == nullptr)
		{
			JUTILS_LOG(error, JSTR("Failed to create shader"));
			return false;
		}
		//JumaRE::Material* material = renderEngine->createMaterial(shader);
		if (material == nullptr)
		{
			JUTILS_LOG(error, JSTR("Failed to create material"));
			renderEngine->destroyAsset(shader);
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
			m_DefaultParamValues = createInfo.defaultValues;
			for (const auto& param : m_MaterialParams)
			{
				resetParamValue(param);
			}
		}

		return true;
	}
	bool MaterialBase::createMaterial(const MaterialBaseCreateInfo& createInfo)
    {
		JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
		OnShaderCreatedTask* task = new OnShaderCreatedTask(this, createInfo);
		m_CreateAsyncTask = task;
		if (!renderEngine->createShaderAsync(createInfo.shaderInfo, task))
		{
			JUTILS_LOG(error, JSTR("Failed to start async task for creating shader"));
			delete m_CreateAsyncTask;
			m_CreateAsyncTask = nullptr;
		    return false;
		}
		return true;
    }
	void MaterialBase::OnShaderCreatedTask::run()
    {
		JumaRE::Shader* shader = getAsset();

		std::lock_guard lock(m_Mutex);
		if (m_Material == nullptr)
		{
		    return;
		}
		if (!m_Material->onShaderCreated_WorkerThread(shader, m_CreateInfo))
		{
			if (shader != nullptr)
			{
				shader->getRenderEngine()->destroyAsset(shader);
			    shader = nullptr;
			}
		}

		NotifyShaderCreatedTask* task = new NotifyShaderCreatedTask(m_Material, shader);
		m_Material->m_CreateAsyncTask = task;
		if (!m_Material->getEngine()->getSubsystem<AsyncEngineSubsystem>()->addTask(AsyncTaskType::GameThread, task))
		{
		    delete m_Material->m_CreateAsyncTask;
			m_Material->m_CreateAsyncTask = nullptr;
		}
    }
	void MaterialBase::NotifyShaderCreatedTask::run()
    {
		if (m_Material == nullptr)
		{
		    return;
		}
		if (m_Material->onShaderCreated_GameThread(m_Shader))
		{
		    m_Material->onAssetCreated.call(m_Material, true);
		}
		else
		{
		    if (m_Shader != nullptr)
			{
				m_Shader->getRenderEngine()->destroyAsset(m_Shader);
			}
			m_Material->onAssetCreated.call(m_Material, false);
		}
		m_Material->onAssetCreated.clear();
    }
	bool MaterialBase::onShaderCreated_WorkerThread(JumaRE::Shader* shader, const MaterialBaseCreateInfo& createInfo)
    {
		m_CreateAsyncTask = nullptr;
		if (shader == nullptr)
		{
		    return false;
		}

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
			m_DefaultParamValues = createInfo.defaultValues;
		}
		return true;
    }
	bool MaterialBase::onShaderCreated_GameThread(JumaRE::Shader* shader)
    {
		m_CreateAsyncTask = nullptr;
		if (shader == nullptr)
		{
		    return false;
		}

		m_Material = getEngine()->getRenderEngine()->createMaterial(shader);
	    if (m_Material == nullptr)
		{
			JUTILS_LOG(error, JSTR("Failed to create material"));
			return false;
		}
		for (const auto& param : m_MaterialParams)
		{
			resetParamValue(param);
		}
		return true;
    }

    void MaterialBase::clearMaterial()
	{
		if (m_CreateAsyncTask != nullptr)
		{
		    m_CreateAsyncTask->clearTask();
			m_CreateAsyncTask = nullptr;
		}

		JumaRE::Shader* shader = getShader();

		Super::clearMaterial();

		if (shader != nullptr)
		{
			getEngine()->getRenderEngine()->destroyAsset(shader);
		}
		m_DefaultParamValues = {};
	}
	void MaterialBase::OnShaderCreatedTask::clearTask()
    {
		m_Mutex.lock();
		m_Material = nullptr;
		m_Mutex.unlock();

		JumaRE::Shader* shader = getAsset();
		if (shader != nullptr)
		{
		    shader->getRenderEngine()->destroyAsset(shader);
		}
    }
	void MaterialBase::NotifyShaderCreatedTask::clearTask()
    {
		m_Material = nullptr;
		if (m_Shader != nullptr)
		{
		    m_Shader->getRenderEngine()->destroyAsset(m_Shader);
		}
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
