﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "AssetsManager.h"
#include "AssetObject.h"
#include "material/Material.h"
#include "material/MaterialInstance.h"
#include "mesh/Mesh.h"
#include "render/RenderManagerBase.h"
#include "render/shader/ShaderBase.h"

namespace JumaEngine
{
    template<typename T>
    void DestroyUnusedAssets(jmap<jstring, asset_ptr<T>>& assetsMap)
    {
    	assetsMap.removeByPredicate([](const jstring& key, const asset_ptr<T>& value)
    	{
    		return (value == nullptr) || (value.use_count() == 1);
    	});
    }

	AssetsManager::~AssetsManager()
	{
        m_MaterialInstances.clear();
        m_Materials.clear();
        m_Meshes.clear();
	}

	void AssetsManager::destroyUnusedAssets()
	{
		DestroyUnusedAssets(m_Materials);
		DestroyUnusedAssets(m_MaterialInstances);
		DestroyUnusedAssets(m_Meshes);
	}

	void AssetsManager::registerAssetObject(const asset_ptr<AssetObject>& asset)
	{
		if (asset != nullptr)
		{
			asset->m_AssetsManager = this;
		}
	}

	asset_ptr<Material> AssetsManager::createMaterial(const jstring& materialName)
	{
		const jstring actualName = jstring(JTEXT("Material.")) + materialName;
		
        asset_ptr<Material>* existingAsset = m_Materials.findByKey(actualName);
		if (existingAsset != nullptr)
		{
			return *existingAsset;
		}

        asset_ptr<Material> asset = createAssetObject<Material>();
        if (asset != nullptr)
        {
            RenderManagerBase* renderManager = getRenderManager();
            asset->m_Shader = renderManager != nullptr ? renderManager->createShader() : nullptr;
            if (asset->m_Shader != nullptr)
            {
                asset->m_Shader->loadShader(materialName);
            }
            m_Materials.add(actualName, asset);
        }
        return asset;
	}
    asset_ptr<MaterialInstance> AssetsManager::createMaterialInstance(const jstring& materialInstanceName, const asset_ptr<MaterialBase>& baseMaterial)
    {
		const jstring actuallName = jstring(JTEXT("MaterialInstance.")) + materialInstanceName;
		
        asset_ptr<MaterialInstance>* existingAsset = m_MaterialInstances.findByKey(actuallName);
		if (existingAsset != nullptr)
		{
			return *existingAsset;
		}

        asset_ptr<MaterialInstance> asset = createAssetObject<MaterialInstance>();
        if (asset != nullptr)
        {
            asset->m_BaseMaterial = baseMaterial;
            m_MaterialInstances.add(actuallName, asset);
        }
        return asset;
    }
    asset_ptr<MaterialInstance> AssetsManager::createMaterialInstance(const asset_ptr<MaterialBase>& baseMaterial)
    {
		if (baseMaterial != nullptr)
		{
			asset_ptr<MaterialInstance> asset = createAssetObject<MaterialInstance>();
			if (asset != nullptr)
			{
				asset->m_BaseMaterial = baseMaterial;
				return asset;
			}
		}
		return nullptr;
    }

    asset_ptr<Mesh> AssetsManager::createMesh(const jstring& meshName, const jarray<VertexBufferDataBase*>& meshPartsData)
    {
        const jstring actuallName = jstring(JTEXT("Mesh.")) + meshName;
		
        asset_ptr<Mesh>* existingAsset = m_Meshes.findByKey(actuallName);
		if (existingAsset != nullptr)
		{
			return *existingAsset;
		}

        asset_ptr<Mesh> asset = createAssetObject<Mesh>();
        if (asset != nullptr)
        {
            asset->initMesh(meshPartsData);
            m_Meshes.add(actuallName, asset);
        }
        return asset;
    }
}
