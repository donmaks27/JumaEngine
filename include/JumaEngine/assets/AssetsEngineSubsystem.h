// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineSubsystem.h"

#include <jutils/jlist.h>

#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "VertexComponent.h"

namespace JumaEngine
{
	class AssetsEngineSubsystem final : public EngineSubsystem
	{
		JUMAENGINE_CLASS(AssetsEngineSubsystem, EngineSubsystem)

	public:
		AssetsEngineSubsystem() = default;
		virtual ~AssetsEngineSubsystem() override = default;
        
        jstring getAssetPath(const jstring& assetID) const;
        
        EngineObjectPtr<Texture> getTextureAsset(const jstringID& assetID);
        EngineObjectPtr<Material> getMaterialAsset(const jstringID& assetID);
        EngineObjectPtr<Material> createMaterial(const EngineObjectPtr<Material>& parentMaterial);
        
        jstringID getVertexComponentID(VertexComponent component) const;
        JumaRE::VertexBuffer* getVertexBuffer_Cube();
        JumaRE::VertexBuffer* getVertexBuffer_Plane2D();

		void clear();

	protected:

		virtual bool initSubsystem() override;
		virtual void clearSubsystem() override;

	private:

        jmap<jstringID, EngineObjectPtr<Asset>> m_LoadedAssets;

        jmap<VertexComponent, jstringID> m_VertexComponentIDs;
        jlist<Mesh> m_Meshes;
        JumaRE::VertexBuffer* m_VertexBuffer_Cube = nullptr;
        JumaRE::VertexBuffer* m_VertexBuffer_Plane2D = nullptr;
        
        jstring m_EngineContentDirectory = JSTR("./content_engine/");
        jstring m_GameContentDirectory = JSTR("./content/");


		void onRenderEngineDestroying(JumaRE::RenderEngine* renderEngine) { clear(); }
        
        EngineObjectPtr<Asset> loadAsset(const jstringID& assetID, AssetType expectedAssetType);
	};
}
