// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "AssetObject.h"
#include "engine/Engine.h"
#include "utils/jmap.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class MaterialBase;
    class Material;
    class MaterialInstance;
    class Mesh;
    class TextureBase;

    class AssetsManager final : public EngineContextObject
	{
        JUMAENGINE_CLASS(AssetsManager, EngineContextObject)

	public:
		AssetsManager() = default;
		virtual ~AssetsManager() override;

		void destroyUnusedAssets();

		asset_ptr<Material> createMaterial(const jstring& materialName);
		asset_ptr<MaterialInstance> createMaterialInstance(const jstring& materialInstanceName, const asset_ptr<MaterialBase>& baseMaterial);
		asset_ptr<MaterialInstance> createMaterialInstance(const asset_ptr<MaterialBase>& baseMaterial);

        asset_ptr<Mesh> createMesh(const jstring& meshName);

        asset_ptr<TextureBase> createTexture(const jstring& textureName);
        asset_ptr<TextureBase> createTexture();
	
	private:

    	jmap<jstring, asset_ptr<Material>> m_Materials;
        jmap<jstring, asset_ptr<MaterialInstance>> m_MaterialInstances;
        jmap<jstring, asset_ptr<Mesh>> m_Meshes;
        jmap<jstring, asset_ptr<TextureBase>> m_Textures;


		template<typename T, TEMPLATE_ENABLE(is_base_and_not_same_and_not_abstract<AssetObject, T>)>
		asset_ptr<T> createAssetObject()
		{
            Engine* engine = getOwnerEngine();
			if (engine != nullptr)
			{
                asset_ptr<T> asset = engine->createObject<T>();
                registerAssetObject(asset);
                return asset;
			}
			return nullptr;
		}
		void registerAssetObject(const asset_ptr<AssetObject>& asset);
	};
}
