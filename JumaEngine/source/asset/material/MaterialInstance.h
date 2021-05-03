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
		virtual void activate(const window_id windowID) const override
		{
			if (m_BaseMaterial != nullptr)
			{
				m_BaseMaterial->activate(windowID);
			}
		}
		virtual bool isActive(const window_id windowID) const override { return (m_BaseMaterial != nullptr) && m_BaseMaterial->isActive(windowID); }
		virtual void deactivate(const window_id windowID) const override
		{
			if (m_BaseMaterial != nullptr)
			{
				m_BaseMaterial->deactivate(windowID);
			}
		}

	protected:

		virtual MaterialBase* getBaseMaterial() const override { return m_BaseMaterial.get(); }

	private:

		asset_ptr<MaterialBase> m_BaseMaterial;
	};
}
