// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MaterialBase.h"

namespace JumaEngine
{
	class Material final : public MaterialBase
	{
        JUMAENGINE_CLASS_OLD(Material, MaterialBase)

		friend AssetsManager;
		
	public:
		Material() = default;
		virtual ~Material() override;

		template<typename T, typename U = T>
		bool addMaterialParam(const jstring& name, const U& value)
		{
			if (!m_Initialized)
			{
                if (addMaterialParamInternal<T, U>(name, value))
                {
                    cacheShaderUniformName(name);
                    return true;
                }
			}
			return false;
		}
		template<typename T, typename U = T>
		bool addMaterialParam(const jstring& name) { return addMaterialParam<T>(name, U()); }

        bool isShaderValid() const;
        jstring getShaderName() const;
        virtual ShaderBase* getShader() const override { return isInit() ? m_Shader : nullptr; }

		void finishInitialization() { m_Initialized = true; }

		virtual bool isInit() const override { return m_Initialized && isShaderValid(); }
		virtual bool activate() const override;
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
