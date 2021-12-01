// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MaterialBaseOld.h"

namespace JumaEngine
{
	class MaterialInstanceOld final : public MaterialBaseOld
	{
        JUMAENGINE_CLASS_OLD(MaterialInstanceOld, MaterialBaseOld)

        friend AssetsManager;

	public:
		MaterialInstanceOld() = default;
		virtual ~MaterialInstanceOld() override = default;

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

		virtual MaterialBaseOld* getBaseMaterial() const override { return m_BaseMaterial.get(); }

	private:

		asset_ptr<MaterialBaseOld> m_BaseMaterial;
	};
}
