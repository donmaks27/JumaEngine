// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MaterialBase.h"

namespace JumaEngine
{
	class ShaderBase;

	class Material final : public MaterialBase
	{
		friend AssetsManager;
		
	public:
		Material() = default;
		virtual ~Material() override;

		template<typename T>
		bool addMaterialParam(const jstring& name, const T& value)
		{
			if (!m_Initialized)
			{
                if (addMaterialParamInternal<T>(name, value))
                {
                    cacheShaderUniformName(name);
                    return true;
                }
			}
			return false;
		}
		template<typename T>
		bool addMaterialParam(const jstring& name) { return addMaterialParam(name, T()); }

        bool isShaderValid() const;
        jstring getShaderName() const;

		void finishInitialization() { m_Initialized = true; }

		virtual bool isInit() const override { return m_Initialized && isShaderValid(); }
		virtual void activate() const override;
		virtual bool isActive() const override;
		virtual void deactivate() const override;

	protected:

		virtual MaterialBase* getBaseMaterial() const override { return nullptr; }
	
	private:
		
        ShaderBase* m_Shader = nullptr;
		bool m_Initialized = false;


        void cacheShaderUniformName(const jstring& name) const;

        void clearShader();
	};
}
