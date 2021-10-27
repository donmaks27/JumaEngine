// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "AssetsManager.h"
#include "AssetObject.h"
#include "material/MaterialOld.h"
#include "material/MaterialInstanceOld.h"
#include "mesh/Mesh.h"
#include "render/RenderManagerBase.h"
#include "render/shader/ShaderBase.h"
#include "render/texture/TextureBase.h"

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
        m_Textures.clear();
	}

	void AssetsManager::destroyUnusedAssets()
	{
		DestroyUnusedAssets(m_Materials);
		DestroyUnusedAssets(m_MaterialInstances);
		DestroyUnusedAssets(m_Meshes);
		DestroyUnusedAssets(m_Textures);
	}

	void AssetsManager::registerAssetObject(const asset_ptr<AssetObject>& asset)
	{
		if (asset != nullptr)
		{
			asset->m_AssetsManager = this;
		}
	}

	asset_ptr<MaterialOld> AssetsManager::createMaterial(const jstring& materialName)
	{
		const jstring actualName = jstring(JSTR("MaterialOld.")) + materialName;
		
        asset_ptr<MaterialOld>* existingAsset = m_Materials.find(actualName);
		if (existingAsset != nullptr)
		{
			return *existingAsset;
		}

        asset_ptr<MaterialOld> asset = createAssetObject<MaterialOld>();
        if (asset != nullptr)
        {
            RenderManagerBase* renderManager = getRenderManager();
            asset->m_Shader = renderManager != nullptr ? renderManager->createShader() : nullptr;
            if (asset->m_Shader != nullptr)
            {
                asset->m_Shader->loadShader(materialName);
            }
            m_Materials.set(actualName, asset);
        }
        return asset;
	}
    asset_ptr<MaterialInstanceOld> AssetsManager::createMaterialInstance(const jstring& materialInstanceName, const asset_ptr<MaterialBaseOld>& baseMaterial)
    {
		const jstring actuallName = jstring(JSTR("MaterialInstanceOld.")) + materialInstanceName;
		
        asset_ptr<MaterialInstanceOld>* existingAsset = m_MaterialInstances.find(actuallName);
		if (existingAsset != nullptr)
		{
			return *existingAsset;
		}

        asset_ptr<MaterialInstanceOld> asset = createMaterialInstance(baseMaterial);
        if (asset != nullptr)
        {
            m_MaterialInstances.set(actuallName, asset);
        }
        return asset;
    }
    asset_ptr<MaterialInstanceOld> AssetsManager::createMaterialInstance(const asset_ptr<MaterialBaseOld>& baseMaterial)
    {
		if (baseMaterial != nullptr)
		{
			asset_ptr<MaterialInstanceOld> asset = createAssetObject<MaterialInstanceOld>();
			if (asset != nullptr)
			{
				asset->m_BaseMaterial = baseMaterial;
				return asset;
			}
		}
		return nullptr;
    }

    asset_ptr<Mesh> AssetsManager::createMesh(const jstring& meshName)
    {
        const jstring actuallName = jstring(JSTR("Mesh.")) + meshName;
		
        asset_ptr<Mesh>* existingAsset = m_Meshes.find(actuallName);
		if (existingAsset != nullptr)
		{
			return *existingAsset;
		}

        asset_ptr<Mesh> asset = createAssetObject<Mesh>();
        if (asset != nullptr)
        {
            m_Meshes.set(actuallName, asset);
        }
        return asset;
    }

    asset_ptr<TextureBase> AssetsManager::createTexture(const jstring& textureName)
    {
        const jstring actuallName = jstring(JSTR("Texture.")) + textureName;
		
        asset_ptr<TextureBase>* existingAsset = m_Textures.find(actuallName);
		if (existingAsset != nullptr)
		{
			return *existingAsset;
		}

        asset_ptr<TextureBase> asset = createTexture();
        if (asset != nullptr)
        {
            m_Textures.set(actuallName, asset);
        }
        return asset;
    }
    asset_ptr<TextureBase> AssetsManager::createTexture()
    {
        RenderManagerBase* renderManager = getRenderManager();
        if (renderManager != nullptr)
        {
            asset_ptr<TextureBase> asset = renderManager->createTexture();
            registerAssetObject(asset);
            return asset;
        }
        return nullptr;
    }
}
