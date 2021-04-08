// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MaterialInstance.h"

namespace JumaEngine
{
	void MaterialInstance::setBaseMaterial(MaterialBase* material)
	{
		if (m_BaseMaterial == nullptr)
		{
			m_BaseMaterial = material;
		}
	}
}
