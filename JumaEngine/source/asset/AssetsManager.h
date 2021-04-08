// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include "utils/jarray.h"
#include "utils/jmap.h"

namespace JumaEngine
{
	class AssetObject;
	class Material;
	
	class AssetsManager : public EngineContextObject
	{
	public:
		AssetsManager() = default;
		virtual ~AssetsManager() override;

		void destroyAsset(AssetObject* asset);

		Material* createMaterial(const jstring& materialName);
	
	private:

		jarray<AssetObject*> m_Assets;
		jmap<jstring, Material*> m_Materials;


		template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<AssetObject, T>)>
		T* createAssetObject()
		{
			if (getOwnerEngine() != nullptr)
			{
				T* asset = new T();
				registerAssetObject(asset);
				return asset;
			}
			return nullptr;
		}
		void registerAssetObject(AssetObject* asset);
	};
}
