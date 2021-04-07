// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MaterialBase.h"

namespace JumaEngine
{
	class MaterialInstance final : public MaterialBase
	{
	public:
		MaterialInstance() = default;
		virtual ~MaterialInstance() override = default;

		void setBaseMaterial(MaterialBase* material);

		virtual MaterialBase* getBaseMaterial() const override { return m_BaseMaterial; }
		
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

	private:

		MaterialBase* m_BaseMaterial = nullptr;
	};
}
