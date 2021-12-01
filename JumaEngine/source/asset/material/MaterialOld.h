// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MaterialBaseOld.h"

namespace JumaEngine
{
	class MaterialOld final : public MaterialBaseOld
	{
        JUMAENGINE_CLASS_OLD(MaterialOld, MaterialBaseOld)

		friend AssetsManager;
		
	public:
		MaterialOld() = default;
		virtual ~MaterialOld() override;

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

		virtual MaterialBaseOld* getBaseMaterial() const override { return nullptr; }
	
	private:
		
        ShaderBase* m_Shader = nullptr;
		bool m_Initialized = false;


        void cacheShaderUniformName(const jstring& name) const;

        void clearShader();
	};
}
