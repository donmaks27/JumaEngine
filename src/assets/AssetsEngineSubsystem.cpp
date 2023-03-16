// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/AssetsEngineSubsystem.h"

#include "AssetFilesParsing.h"
#include "JumaEngine/assets/MaterialInstance.h"
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
                    for (auto& shaderFile : createInfo.shaderFiles)
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
}
