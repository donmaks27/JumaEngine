// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MaterialBase.h"

namespace JumaEngine
{
	class MaterialInstance final : public MaterialBase
	{
        JUMAENGINE_CLASS_OLD(MaterialInstance, MaterialBase)

        friend AssetsManager;

	public:
		MaterialInstance() = default;
		virtual ~MaterialInstance() override = default;

        virtual ShaderBase* getShader() const override { return m_BaseMaterial != nullptr ? m_BaseMaterial->getShader() : nullptr; }
		
		virtual bool isInit() const override { return (m_BaseMaterial != nullptr) && m_BaseMaterial->isInit(); }
		virtual bool activate() const override
		{
            return m_BaseMaterial != nullptr ? m_BaseMaterial->activate() : false;
		}
		virtual void deactivate() const override
		{
			if (m_BaseMaterial != nullptr)
			{
				m_BaseMaterial->deactivate();
			}
		}

	protected:

		virtual MaterialBase* getBaseMaterial() const override { return m_BaseMaterial.get(); }

	private:

		asset_ptr<MaterialBase> m_BaseMaterial;
	};
}
