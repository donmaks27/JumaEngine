// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MaterialBase.h"

namespace JumaEngine
{
	class ShaderBase;

	class Material final : public MaterialBase
	{
	public:
		Material() = default;
		virtual ~Material() override;

		virtual MaterialBase* getBaseMaterial() const override { return nullptr; }

		template<typename T>
		bool addMaterialParam(const jstring& name, const T& value)
		{
			if (!m_Initialized)
			{
				return addMaterialParamInternal<T>(name, value);
			}
			return false;
		}
		template<typename T>
		bool addMaterialParam(const jstring& name) { return addMaterialParam(name, T()); }

        bool isShaderValid() const;
        jstring getShaderName() const;
        void setShaderName(const jstring& shaderName);
        void clearShader();

		void finishInitialization() { m_Initialized = true; }

		virtual bool isInit() const override { return m_Initialized && isShaderValid(); }
		virtual void activate() const override;
		virtual bool isActive() const override;
		virtual void deactivate() const override;
	
	private:
		
        ShaderBase* m_Shader = nullptr;

		bool m_Initialized = false;
	};
}
