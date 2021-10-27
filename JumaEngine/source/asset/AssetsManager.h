// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "AssetObject.h"
#include "engine/Engine.h"
#include "utils/jmap.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class MaterialBaseOld;
    class MaterialOld;
    class MaterialInstanceOld;
    class Mesh;
    class TextureBase;

    class AssetsManager final : public EngineContextObjectOld
	{
        JUMAENGINE_CLASS_OLD(AssetsManager, EngineContextObjectOld)

	public:
		AssetsManager() = default;
		virtual ~AssetsManager() override;

		void destroyUnusedAssets();

		asset_ptr<MaterialOld> createMaterial(const jstring& materialName);
		asset_ptr<MaterialInstanceOld> createMaterialInstance(const jstring& materialInstanceName, const asset_ptr<MaterialBaseOld>& baseMaterial);
		asset_ptr<MaterialInstanceOld> createMaterialInstance(const asset_ptr<MaterialBaseOld>& baseMaterial);

        asset_ptr<Mesh> createMesh(const jstring& meshName);

        asset_ptr<TextureBase> createTexture(const jstring& textureName);
        asset_ptr<TextureBase> createTexture();
	
	private:

    	jmap<jstring, asset_ptr<MaterialOld>> m_Materials;
        jmap<jstring, asset_ptr<MaterialInstanceOld>> m_MaterialInstances;
        jmap<jstring, asset_ptr<Mesh>> m_Meshes;
        jmap<jstring, asset_ptr<TextureBase>> m_Textures;


		template<typename T, TEMPLATE_ENABLE(is_base_and_not_same_and_not_abstract<AssetObject, T>)>
		asset_ptr<T> createAssetObject()
		{
            EngineOld* engine = getOwnerEngine();
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
