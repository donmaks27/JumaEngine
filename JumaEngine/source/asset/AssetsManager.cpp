// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "AssetsManager.h"
#include "AssetObject.h"
#include "material/Material.h"
#include "material/MaterialInstance.h"
#include "utils/system_functions.h"

namespace JumaEngine
{
	AssetsManager::~AssetsManager()
	{
        m_MaterialInstances.clear();
        m_NamedMaterialInstances.clear();
		m_Materials.clear();

        for (auto& asset : m_Assets)
        {
            if (asset != nullptr)
            {
                asset->terminate();
            }
        }
		m_Assets.clear();
	}

	void AssetsManager::registerAssetObject(const std::shared_ptr<AssetObject>& asset)
	{
		if (asset != nullptr)
		{
			m_Assets.add(asset);
			asset->m_AssetsManager = this;
		}
	}

	void AssetsManager::destroyAsset(const asset_ptr<AssetObject>& assetPtr)
	{
        AssetObject* asset = assetPtr.get();
		if (asset != nullptr)
		{
			asset->terminate();
            for (int32 index = 0; index < m_Assets.size(); index++)
            {
                if (m_Assets[index].get() == asset)
                {
                    m_Assets.removeAt(index);
                    assetPtr.updatePtr();
                    return;
                }
            }
		}
	}

	asset_ptr<Material> AssetsManager::createMaterial(const jstring& materialName)
	{
        asset_ptr<Material>* assetPtrPtr = m_Materials.findByKey(materialName);
		if (assetPtrPtr != nullptr)
		{
			return *assetPtrPtr;
		}

        asset_ptr<Material> assetPtr = createAssetObject<Material>();
        Material* material = assetPtr.get();
        if (material != nullptr)
        {
            material->m_Shader = SystemFunctions::createShader(this, materialName);
            m_Materials.add(materialName, assetPtr);
        }
        return assetPtr;
	}
    asset_ptr<MaterialInstance> AssetsManager::createMaterialInstance(const jstring& materialInstanceName, const asset_ptr<MaterialBase>& baseMaterial)
    {
        asset_ptr<MaterialInstance>* assetPtrPtr = m_NamedMaterialInstances.findByKey(materialInstanceName);
		if (assetPtrPtr != nullptr)
		{
			return *assetPtrPtr;
		}

        asset_ptr<MaterialInstance> assetPtr = createAssetObject<MaterialInstance>();
        MaterialInstance* materialInstance = assetPtr.get();
        if (materialInstance != nullptr)
        {
            materialInstance->m_BaseMaterial = baseMaterial;
            m_NamedMaterialInstances.add(materialInstanceName, assetPtr);
        }
        return assetPtr;
    }
    asset_ptr<MaterialInstance> AssetsManager::createMaterialInstance(const asset_ptr<MaterialBase>& baseMaterial)
    {
        asset_ptr<MaterialInstance> assetPtr = createAssetObject<MaterialInstance>();
        MaterialInstance* materialInstance = assetPtr.get();
        if (materialInstance != nullptr)
        {
            materialInstance->m_BaseMaterial = baseMaterial;
            m_MaterialInstances.add(assetPtr);
        }
        return assetPtr;
    }
}
