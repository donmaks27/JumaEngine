// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MaterialBase.h"

namespace JumaEngine
{
	class MaterialInstance final : public MaterialBase
	{
        friend AssetsManager;

	public:
		MaterialInstance() = default;
		virtual ~MaterialInstance() override = default;
		
		virtual bool isInit() const override { return (m_BaseMaterial != nullptr) && m_BaseMaterial->isInit(); }
		virtual void activate() const override
		{
			if (m_BaseMaterial != nullptr)
			{
				m_BaseMaterial->activate();
			}
		}
		virtual bool isActive() const override { return (m_BaseMaterial != nullptr) && m_BaseMaterial->isActive(); }
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
