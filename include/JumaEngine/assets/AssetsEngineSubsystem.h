// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineSubsystem.h"

#include <jutils/jasync_task_queue.h>

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
        
        bool getAssetAsync(EngineContextObject* context, const jstringID& assetID, std::function<void(const EngineObjectPtr<Asset>&)> callback);
        EngineObjectPtr<Material> createMaterial(const EngineObjectPtr<Material>& parentMaterial);

        jstringID getVertexComponentID(VertexComponent component) const;
        JumaRE::VertexBuffer* getVertexBuffer_Cube();
        JumaRE::VertexBuffer* getVertexBuffer_Plane2D();

		void clear();

	protected:

		virtual bool initSubsystem() override;
		virtual void clearSubsystem() override;

	private:

        class AsyncAssetCreateTask : public jasync_task
        {
        public:
            AsyncAssetCreateTask() = delete;
            AsyncAssetCreateTask(AssetsEngineSubsystem* subsystem, const jstringID& assetID)
                : m_Subsystem(subsystem), m_AssetID(assetID)
            {}

            void addCallback(const EngineObjectWeakPtr<EngineContextObject>& context, std::function<void(const EngineObjectPtr<Asset>&)>&& callback)
            {
                m_Callbacks.putBack(context, std::move(callback));
            }
            void notify(bool success);

            virtual void run() override;
            virtual bool shouldDeleteAfterExecution() const override { return false; }

            const EngineObjectPtr<Asset>& getAsset() const { return m_Asset; }

        private:
        
            jarray<std::pair<EngineObjectWeakPtr<EngineContextObject>, std::function<void(const EngineObjectPtr<Asset>&)>>> m_Callbacks;
            AssetsEngineSubsystem* m_Subsystem = nullptr;
            jstringID m_AssetID = jstringID_NONE;

            std::function<bool()> m_AssetCreateFunction = nullptr;
            EngineObjectPtr<Asset> m_Asset = nullptr;
            jstring m_ErrorMessage = jstring();

            bool m_AssetDataLoaded = false;


            bool readAssetData();
        };

        jmap<jstringID, EngineObjectPtr<Asset>> m_LoadedAssets;
        jmap<jstringID, AsyncAssetCreateTask> m_LoadAssetTasks;

        jmap<VertexComponent, jstringID> m_VertexComponentIDs;
        jlist<Mesh> m_Meshes;
        JumaRE::VertexBuffer* m_VertexBuffer_Cube = nullptr;
        JumaRE::VertexBuffer* m_VertexBuffer_Plane2D = nullptr;
        
        jstring m_EngineContentDirectory = JSTR("./content_engine/");
        jstring m_GameContentDirectory = JSTR("./content/");


		bool prepareAssetForCreation(const EngineObjectPtr<Asset>& asset, const jstringID& assetID, bool subscribe = true);
        void notifyAssetCreatedOnNextTick(const jstringID& assetID, bool success);
        void onAssetCreated(Asset* asset, bool success);
        void onAssetCreateTaskFinished(const jstringID& assetID, bool success);
        
        void onRenderEngineDestroying(JumaRE::RenderEngine* renderEngine) { clear(); }
	};
}
