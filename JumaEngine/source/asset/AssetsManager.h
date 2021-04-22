// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include "AssetObject.h"
#include "utils/jmap.h"

namespace JumaEngine
{
    class MaterialBase;
    class Material;
    class MaterialInstance;

    class AssetsManager final : public EngineContextObject
	{
	public:
		AssetsManager() = default;
		virtual ~AssetsManager() override;

		void destroyUnusedAssets();

		asset_ptr<Material> createMaterial(const jstring& materialName);
		asset_ptr<MaterialInstance> createMaterialInstance(const jstring& materialInstanceName, const asset_ptr<MaterialBase>& baseMaterial);
		asset_ptr<MaterialInstance> createMaterialInstance(const asset_ptr<MaterialBase>& baseMaterial);
	
	private:

    	jmap<jstring, asset_ptr<Material>> m_Materials;
        jmap<jstring, asset_ptr<MaterialInstance>> m_MaterialInstances;


		template<typename T, TEMPLATE_ENABLE(is_base_and_not_same_and_not_abstract<AssetObject, T>)>
		std::shared_ptr<T> createAssetObject()
		{
			if (getOwnerEngine() != nullptr)
			{
                std::shared_ptr<T> asset = std::make_shared<T>();
                registerAssetObject(asset);
                return asset;
			}
			return nullptr;
		}
		void registerAssetObject(const std::shared_ptr<AssetObject>& asset);

    	template<typename T>
    	void destroyUnusedAssets(jmap<jstring, asset_ptr<T>>& assetsMap)
    	{
    		assetsMap.removeByPredicate([](const jstring& key, const asset_ptr<T>& value)
    		{
    			return (value == nullptr) || (value.use_count() == 1);
    		});
    	}
	};
}
