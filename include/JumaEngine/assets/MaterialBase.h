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
		JumaRE::ShaderCreateInfo shaderInfo;

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

		virtual const Material* getBaseMaterial() const override { return this; }

		virtual bool resetParamValueInternal(const jstringID& paramName, const jstringID& uniformName) override;

		virtual void clearMaterial() override;

	private:

		class MaterialBaseAsyncTask
		{
		protected:
			MaterialBaseAsyncTask() = default;
		public:
			virtual ~MaterialBaseAsyncTask() = default;
			
			virtual void clearTask() = 0;
		};
		class OnShaderCreatedTask final : public JumaRE::RenderEngine::OnAssetCreatedTask<JumaRE::Shader>, public MaterialBaseAsyncTask
		{
		public:
			OnShaderCreatedTask() = default;
			OnShaderCreatedTask(MaterialBase* ptr, const MaterialBaseCreateInfo& createInfo)
			    : m_CreateInfo(createInfo)
		        , m_Material(ptr)
			{}

			virtual void run() override;
			virtual void clearTask() override;

		private:
			
			std::mutex m_Mutex;
			MaterialBaseCreateInfo m_CreateInfo;
			MaterialBase* m_Material = nullptr;
		};
		class NotifyShaderCreatedTask final : public jasync_task, public MaterialBaseAsyncTask
		{
		public:
			NotifyShaderCreatedTask() = default;
			NotifyShaderCreatedTask(MaterialBase* material, JumaRE::Shader* shader)
			    : m_Material(material), m_Shader(shader)
			{}

			virtual void run() override;
			virtual void clearTask() override;

		private:

			MaterialBase* m_Material = nullptr;
			JumaRE::Shader* m_Shader = nullptr;
		};

		MaterialDefaultParamValues m_DefaultParamValues;

		MaterialBaseAsyncTask* m_CreateAsyncTask = nullptr;


		bool loadMaterial(const MaterialBaseCreateInfo& createInfo);
		bool createMaterial(const MaterialBaseCreateInfo& createInfo);
		bool onShaderCreated_WorkerThread(JumaRE::Shader* shader, const MaterialBaseCreateInfo& createInfo);
		bool onShaderCreated_GameThread(JumaRE::Shader* shader);

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
		template<>
		bool updateDefaultParamValue<MaterialParamType::Texture>(const jstringID& paramName, const jstringID& uniformName);
	};
}
