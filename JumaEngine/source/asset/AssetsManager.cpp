// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "AssetsManager.h"
#include "AssetObject.h"
#include "material/Material.h"
#include "utils/system_functions.h"

namespace JumaEngine
{
	AssetsManager::~AssetsManager()
	{
		m_Materials.clear();
		
		for (auto& asset : m_Assets)
		{
			asset->terminate();
			delete asset;
		}
		m_Assets.clear();
	}

	void AssetsManager::destroyAsset(AssetObject* asset)
	{
		if (asset != nullptr)
		{
			asset->terminate();
			m_Assets.remove(asset);

			delete asset;
		}
	}

	void AssetsManager::registerAssetObject(AssetObject* asset)
	{
		if (asset != nullptr)
		{
			m_Assets.add(asset);
			asset->m_AssetsManager = this;
		}
	}

	Material* AssetsManager::createMaterial(const jstring& materialName)
	{
		Material** materialPtr = m_Materials.findByKey(materialName);
		if (materialPtr != nullptr)
		{
			return *materialPtr;
		}

		Material* material = createAssetObject<Material>();
		if (material != nullptr)
		{
			material->m_Shader = SystemFunctions::createShader(this, materialName);
			m_Materials.add(materialName, material);
			return material;
		}

		return nullptr;
	}
}
