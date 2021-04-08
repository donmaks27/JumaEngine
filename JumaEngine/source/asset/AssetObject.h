// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
	class AssetsManager;
	
	class AssetObject
	{
		friend AssetsManager;
		
	protected:
		AssetObject() = default;
		virtual ~AssetObject() = default;

		virtual void terminate() {}

	private:

		AssetsManager* m_AssetsManager = nullptr;
	};
}
