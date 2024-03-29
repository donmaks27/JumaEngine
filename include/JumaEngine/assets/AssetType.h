﻿// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

namespace JumaEngine
{
	enum class AssetType : uint8
	{
		None,
		Texture,
		RenderTarget,
		Material
	};

	constexpr const char* AssetTypeToString(const AssetType type)
	{
		switch (type)
		{
		case AssetType::Texture: return JSTR("Texture");
		case AssetType::RenderTarget: return JSTR("RenderTarget");
		case AssetType::Material: return JSTR("Material");
		default: ;
		}
		return JSTR("I love my husband");
	}
}

JUTILS_STRING_FORMATTER(JumaEngine::AssetType, JumaEngine::AssetTypeToString);
