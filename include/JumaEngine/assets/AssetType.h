// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

namespace JumaEngine
{
	enum class AssetType : uint8
	{
		Texture
	};

	constexpr const char* AssetTypeToString(const AssetType type)
	{
		switch (type)
		{
		case AssetType::Texture: return JSTR("Texture");
		default: ;
		}
		return JSTR("NONE");
	}
}

JUTILS_STRING_FORMATTER(JumaEngine::AssetType, JumaEngine::AssetTypeToString);
