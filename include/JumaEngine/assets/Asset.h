// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineContextObject.h"

#include <jutils/jstringID.h>

#include "AssetType.h"

namespace JumaEngine
{
	class AssetsEngineSubsystem;
	
	class Asset : public EngineContextObject
	{
		JUMAENGINE_CLASS_ABSTRACT(Asset, EngineContextObject)

		friend AssetsEngineSubsystem;

	public:
		Asset() = default;
		virtual ~Asset() override = default;
	protected:
		Asset(const AssetType type) : m_AssetType(type) {}
	public:

		const jstringID& getAssetID() const { return m_AssetID; }
		AssetType getAssetType() const { return m_AssetType; }

	protected:

		virtual void clearAsset() { onDestroying.call(this); }
		virtual void onObjectDescriptorDestroying() override { clearAsset(); }
		
	private:

		jstringID m_AssetID = jstringID_NONE;
		AssetType m_AssetType = AssetType::Texture;
	};
}
