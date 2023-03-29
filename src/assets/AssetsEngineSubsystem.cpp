﻿// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/AssetsEngineSubsystem.h"

#include "AssetFilesParsing.h"
#include "JumaEngine/assets/MaterialInstance.h"
#include "JumaEngine/engine/AsyncEngineSubsystem.h"
#include "JumaEngine/engine/ConfigEngineSubsystem.h"
#include "JumaEngine/engine/Engine.h"
#include "JumaEngine/render/RenderEngineSubsystem.h"

namespace JumaEngine
{
	bool AssetsEngineSubsystem::initSubsystem()
	{
		if (!Super::initSubsystem())
        {
            return false;
        }

		const ConfigEngineSubsystem* configSubsystem = getEngine()->getSubsystem<ConfigEngineSubsystem>();
        if (configSubsystem == nullptr)
        {
            JUTILS_LOG(error, JSTR("Can't find ConfigEngineSubsystem"));
	        return false;
        }
        const jstringID section = JSTR("General");
        const jstringID key = JSTR("contentFolder");
        configSubsystem->getValue(JSTR("engine"), section, key, m_EngineContentDirectory);
        configSubsystem->getValue(JSTR("game"), section, key, m_GameContentDirectory);
        if (m_EngineContentDirectory == m_GameContentDirectory)
        {
            JUTILS_LOG(error, JSTR("Engine content directory is the same as game content directory ({})"), m_EngineContentDirectory);
	        return false;
        }

        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        renderEngine->registerVertexComponent(m_VertexComponentIDs.add(VertexComponent::Position2D, JSTR("position2D")), { 
            JumaRE::VertexComponentType::Vec2, 0
        });
        renderEngine->registerVertexComponent(m_VertexComponentIDs.add(VertexComponent::Position3D, JSTR("position3D")), { 
            JumaRE::VertexComponentType::Vec3, 0
        });
        renderEngine->registerVertexComponent(m_VertexComponentIDs.add(VertexComponent::TextureCoords, JSTR("textureCoords")), { 
            JumaRE::VertexComponentType::Vec2, 1
        });

        renderEngine->onDestroying.bind(this, &AssetsEngineSubsystem::onRenderEngineDestroying);
        return true;
	}
	void AssetsEngineSubsystem::clearSubsystem()
	{
		clear();
		Super::clearSubsystem();
	}
	void AssetsEngineSubsystem::clear()
	{
		JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        if (renderEngine != nullptr)
        {
            renderEngine->onDestroying.unbind(this, &AssetsEngineSubsystem::onRenderEngineDestroying);
        }

        for (auto& mesh : m_Meshes)
        {
            mesh.clearMesh();
        }
        m_Meshes.clear();

        renderEngine->destroyVertexBuffer(m_VertexBuffer_Cube);
        renderEngine->destroyVertexBuffer(m_VertexBuffer_Plane2D);
        m_VertexBuffer_Cube = nullptr;
        m_VertexBuffer_Plane2D = nullptr;
        
        for (const auto& asset : m_LoadedAssets)
        {
	        asset.value->clearAsset();
        }
        m_LoadedAssets.clear();
	}
    
    jstringID AssetsEngineSubsystem::getVertexComponentID(const VertexComponent component) const
	{
        const jstringID* componentID = m_VertexComponentIDs.find(component);
        return componentID != nullptr ? *componentID : jstringID_NONE;
	}
    JumaRE::VertexBuffer* AssetsEngineSubsystem::getVertexBuffer_Cube()
    {
        if (m_VertexBuffer_Cube != nullptr)
        {
	        return m_VertexBuffer_Cube;
        }

        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        if (renderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Can't get render engine"));
            return nullptr;
        }

        struct vertex_data
        {
            math::vector3 position;
            math::vector2 textureCoords;
        };
        const jarray<vertex_data> data = {
            { { -1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f } },
            { { -1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f } },
            { {  1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f } },
            { {  1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f } },
            
            { {  1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f } },
            { {  1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f } },
            { {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f } },
            { {  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f } },
            
            { {  1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { {  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f } },
            { { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f } },
            { { -1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f } },
            
            { { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f } },
            { { -1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f } },
            { { -1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f } },
            
            { { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { { -1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f } },
            { {  1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f } },
            { {  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f } },
            
            { { -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f } },
            { { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f } },
            { {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f } },
            { {  1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f } }
        };
        const jarray<uint32> indices = {
             0,  1,  2,  0,  2,  3,
             4,  5,  6,  4,  6,  7,
             8,  9, 10,  8, 10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23
        };

        m_VertexBuffer_Cube = renderEngine->createVertexBuffer(JumaRE::MakeVertexBufferData({ {
            getVertexComponentID(VertexComponent::Position3D), getVertexComponentID(VertexComponent::TextureCoords)
        } }, data, indices ));
        return m_VertexBuffer_Cube;
    }
    JumaRE::VertexBuffer* AssetsEngineSubsystem::getVertexBuffer_Plane2D()
    {
        if (m_VertexBuffer_Plane2D != nullptr)
        {
	        return m_VertexBuffer_Plane2D;
        }

        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        if (renderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Can't get render engine"));
            return nullptr;
        }

        struct vertex_data
        {
            math::vector2 position;
            math::vector2 textureCoords;
        };
        const jarray<vertex_data> data = {
            { jutils::math::vector2{ 0.0f, 0.0f }, { 0.0f, 0.0f } },
            { jutils::math::vector2{ 0.0f, 1.0f }, { 0.0f, 1.0f } },
            { jutils::math::vector2{ 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { jutils::math::vector2{ 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { jutils::math::vector2{ 0.0f, 1.0f }, { 0.0f, 1.0f } },
            { jutils::math::vector2{ 1.0f, 1.0f }, { 1.0f, 1.0f } }
        };

        m_VertexBuffer_Plane2D = renderEngine->createVertexBuffer(JumaRE::MakeVertexBufferData({ {
            getVertexComponentID(VertexComponent::Position2D), getVertexComponentID(VertexComponent::TextureCoords)
        } }, data));
        return m_VertexBuffer_Plane2D;
    }

    jstring AssetsEngineSubsystem::getAssetPath(const jstring& assetID) const
	{
        if ((assetID.getSize() <= 2) || (assetID[1] != ':'))
	    {
		    return {};
	    }
	    switch (assetID[0])
	    {
	    case 'e': return m_EngineContentDirectory + assetID.substr(2);
	    case 'g': return m_GameContentDirectory + assetID.substr(2);
        default: ;
	    }
        return {};
	}

    EngineObjectPtr<Asset> AssetsEngineSubsystem::getAsset(const jstringID& assetID)
    {
        const jstring assetIDStr = assetID.toString();
        const EngineObjectPtr<Asset>* assetPtr = m_LoadedAssets.find(assetID);
        if (assetPtr != nullptr)
        {
	        return *assetPtr;
        }

        jstring assetPath = getAssetPath(assetIDStr);
        if (assetPath.isEmpty())
        {
            JUTILS_LOG(warning, JSTR("Invalid asset ID {}"), assetIDStr);
	        return nullptr;
        }
        assetPath += JSTR(".json");

        AssetType type;
        json::json_value config = nullptr;
        if (!LoadAssetFile(assetPath, type, config))
        {
	        JUTILS_LOG(warning, JSTR("Failed to load asset {}"), assetIDStr);
	        return nullptr;
        }

        EngineObjectPtr<Asset> assetObject = nullptr;
        switch (type)
        {
        case AssetType::Texture:
	        {
		        TextureAssetCreateInfo createInfo;
		        if (!ParseTextureAssetFile(assetPath, config, createInfo))
		        {
		            JUTILS_LOG(warning, JSTR("Failed parse asset {}"), assetIDStr);
			        return nullptr;
		        }

                EngineObjectPtr<Texture> texture = getEngine()->createObject<Texture>();
		        if ((texture == nullptr) || !texture->loadAsset(createInfo))
		        {
			        JUTILS_LOG(error, JSTR("Failed to create texture asset {}"), assetIDStr);
		            return nullptr;
		        }
                assetObject = std::move(texture);
	        }
            break;
        case AssetType::RenderTarget:
            {
                RenderTargetCreateInfo createInfo;
                if (!ParseRenderTargetAssetFile(assetPath, config, createInfo))
                {
                    JUTILS_LOG(warning, JSTR("Failed parse asset {}"), assetIDStr);
			        return nullptr;
                }
                RenderEngineSubsystem* renderSubsystem = getEngine()->getSubsystem<RenderEngineSubsystem>();
                EngineObjectPtr<RenderTarget> renderTarget = renderSubsystem != nullptr ? renderSubsystem->createRenderTarget(createInfo) : nullptr;
                if (renderTarget == nullptr)
                {
                    JUTILS_LOG(error, JSTR("Failed to create render target asset {}"), assetIDStr);
		            return nullptr;
                }
                assetObject = std::move(renderTarget);
            }
            break;
        case AssetType::Material:
	        {
                static const jstringID parentStringID = JSTR("parentMaterial");
                const json::json_value* parentMaterialValue = config->asObject().find(parentStringID);
		        if (parentMaterialValue != nullptr)
		        {
                    MaterialInstanceCreateInfo createInfo;
                    if (!ParseMaterialInstanceAssetFile(assetPath, config, getAsset<Material>((*parentMaterialValue)->asString()), createInfo))
                    {
                        return nullptr;
                    }
                    EngineObjectPtr<MaterialInstance> material = getEngine()->createObject<MaterialInstance>();
                    if ((material == nullptr) || !material->createMaterial(createInfo))
                    {
                        return nullptr;
                    }
			        assetObject = std::move(material);
		        }
                else
                {
                    MaterialBaseCreateInfo createInfo;
	                if (!ParseMaterialAssetFile(assetPath, config, getEngine()->getRenderEngine()->getRenderAPI(), createInfo))
	                {
		                JUTILS_LOG(error, JSTR("Failed to parse material asset file {}"), assetIDStr);
						return nullptr;
	                }
                    for (auto& shaderFile : createInfo.shaderInfo.fileNames)
                    {
	                    shaderFile.value = getAssetPath(shaderFile.value);
                    }

                    EngineObjectPtr<MaterialBase> material = getEngine()->createObject<MaterialBase>();
                    if ((material == nullptr) || !material->loadMaterial(createInfo))
                    {
	                    JUTILS_LOG(error, JSTR("Failed to load material asset {}"), assetIDStr);
						return nullptr;
                    }
                    assetObject = std::move(material);
                }
	        }
            break;
        default: ;
        }

        JUTILS_LOG(correct, JSTR("Loaded asset {} ({})"), assetIDStr, type);
        assetObject->m_AssetID = assetID;
        m_LoadedAssets.add(assetID, assetObject);
        return assetObject;
    }
    EngineObjectPtr<Material> AssetsEngineSubsystem::createMaterial(const EngineObjectPtr<Material>& parentMaterial)
    {
        if (parentMaterial == nullptr)
        {
	        return nullptr;
        }
        EngineObjectPtr<MaterialInstance> material = getEngine()->createObject<MaterialInstance>();
        if ((material == nullptr) || !material->createMaterial({ parentMaterial }))
        {
	        JUTILS_LOG(error, JSTR("Failed to create material from {}"), parentMaterial->getAssetID().toString());
			return nullptr;
        }
        return material;
    }

    bool AssetsEngineSubsystem::getAssetAsync(EngineContextObject* context, const jstringID& assetID, std::function<void(const EngineObjectPtr<Asset>&)> callback)
    {
        EngineObjectWeakPtr weakContext(context);
        if ((weakContext == nullptr) || (callback == nullptr))
        {
            return false;
        }

        EngineObjectPtr<Asset>* loadedAsset = m_LoadedAssets.find(assetID);
        if (loadedAsset != nullptr)
        {
            getEngine()->getSubsystem<AsyncEngineSubsystem>()->addTask(AsyncTaskType::GameThread, new jasync_task_default([weakContext, callback, asset = *loadedAsset]()
            {
                if (weakContext != nullptr)
                {
                    callback(asset);
                }
            }));
            return true;
        }
        AsyncAssetCreateTask* loadTask = m_LoadAssetTasks.find(assetID);
        if (loadTask != nullptr)
        {
            loadTask->addCallback(weakContext, std::move(callback));
            return true;
        }

        AsyncAssetCreateTask* task = &m_LoadAssetTasks.put(assetID, this, assetID);
        task->addCallback(weakContext, std::move(callback));
        if (!getEngine()->getSubsystem<AsyncEngineSubsystem>()->addTask(AsyncTaskType::Worker, task))
        {
            m_LoadAssetTasks.remove(assetID);
            JUTILS_LOG(error, JSTR("Failed to start loading asset {}"), assetID);
            return false;
        }
        return true;
    }
    void AssetsEngineSubsystem::AsyncAssetCreateTask::run()
    {
        if (!m_AssetDataLoaded)
        {
            readAssetData();
            m_AssetDataLoaded = true;
            m_Subsystem->getEngine()->getSubsystem<AsyncEngineSubsystem>()->addTask(AsyncTaskType::GameThread, this);
        }
        else
        {
            if (!m_ErrorMessage.isEmpty())
            {
                JUTILS_LOG(error, *m_ErrorMessage);
                m_Subsystem->onAssetCreateFailed(m_AssetID);
            }
            else if ((m_AssetCreateFunction == nullptr) || !m_AssetCreateFunction())
            {
                m_Subsystem->onAssetCreateFailed(m_AssetID);
            }
        }
    }

    bool AssetsEngineSubsystem::AsyncAssetCreateTask::readAssetData()
    {
        const jstring assetIDStr = m_AssetID.toString();
        jstring assetPath = m_Subsystem->getAssetPath(assetIDStr);
        if (assetPath.isEmpty())
        {
            return false;
        }
        assetPath += JSTR(".json");

        AssetType type;
        json::json_value config = nullptr;
        if (!LoadAssetFile(assetPath, type, config, m_ErrorMessage))
        {
            return false;
        }

        switch (type)
        {
        case AssetType::Material:
            {
                static const jstringID parentStringID = JSTR("parentMaterial");
                const json::json_value* parentMaterialValue = config->asObject().find(parentStringID);
                if (parentMaterialValue == nullptr)
                {
                    MaterialBaseCreateInfo createInfo;
                    const JumaRE::RenderAPI renderAPI = m_Subsystem->getEngine()->getRenderEngine()->getRenderAPI();
                    if (!ParseMaterialAssetFile(assetPath, config, renderAPI, createInfo, m_ErrorMessage))
                    {
                        return false;
                    }
                    for (auto& shaderFile : createInfo.shaderInfo.fileNames)
                    {
	                    shaderFile.value = m_Subsystem->getAssetPath(shaderFile.value);
                    }
                    m_AssetCreateFunction = [this, createInfo]() -> bool
                    {
                        EngineObjectPtr<MaterialBase> material = m_Subsystem->getEngine()->createObject<MaterialBase>();
                        m_Asset = material;
                        if (!m_Subsystem->prepareAssetForCreation(m_Asset, m_AssetID) || !material->createMaterial(createInfo))
                        {
                            m_Asset = nullptr;
                            return false;
                        }
                        return true;
                    };
                    return true;
                }
            }
            break;
        
        default: ;
        }
        return false;
    }
    bool AssetsEngineSubsystem::prepareAssetForCreation(const EngineObjectPtr<Asset>& asset, const jstringID& assetID)
    {
        if (asset == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create asset {}"), assetID);
            return false;
        }
        asset->m_AssetID = assetID;
        asset->onAssetCreated.bind(this, &AssetsEngineSubsystem::onAssetCreated);
        return true;
    }

    void AssetsEngineSubsystem::onAssetCreateFailed(const jstringID& assetID)
    {
        getEngine()->getSubsystem<AsyncEngineSubsystem>()->addTask(AsyncTaskType::GameThread, new jasync_task_default([this, assetID = assetID]()
        {
            onAssetCreateTaskFinished(assetID, false);
        }));
    }
    void AssetsEngineSubsystem::onAssetCreated(Asset* asset, const bool success)
    {
        if (asset != nullptr)
        {
            onAssetCreateTaskFinished(asset->getAssetID(), success);
        }
    }
    void AssetsEngineSubsystem::onAssetCreateTaskFinished(const jstringID& assetID, bool success)
    {
        AsyncAssetCreateTask* task = m_LoadAssetTasks.find(assetID);
        if (task == nullptr)
        {
            return;
        }

        m_LoadedAssets.add(assetID, success ? task->getAsset() : nullptr);
        task->notify(success);
        m_LoadAssetTasks.remove(assetID);
    }
    void AssetsEngineSubsystem::AsyncAssetCreateTask::notify(const bool success)
    {
        for (const auto& callback : m_Callbacks)
        {
            if ((callback.first != nullptr) && (callback.second != nullptr))
            {
                callback.second(success ? m_Asset : nullptr);
            }
        }
        m_Callbacks.clear();
    }
}
