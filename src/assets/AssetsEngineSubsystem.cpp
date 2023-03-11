// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/AssetsEngineSubsystem.h"

#include <jutils/configs/json_parser.h>

#include "JumaEngine/engine/ConfigEngineSubsystem.h"
#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
    bool LoadAssetFile(const jstring& assetPath, AssetType& outAssetType, json::json_value& outConfig)
    {
        static const jstringID assetTypeField = JSTR("assetType");
        static const jstringID assetTypeTexture = JSTR("texture");

	    const json::json_value config = json::parseFile(assetPath);
        if (config == nullptr)
        {
            JUTILS_LOG(warning, JSTR("Can't find asset file {}"), assetPath);
	        return false;
        }
        const jmap<jstringID, json::json_value>* jsonObject = nullptr;
        if (!config->tryGetObject(jsonObject))
        {
            JUTILS_LOG(warning, JSTR("Invalid asset file {}"), assetPath);
	        return false;
        }
        
        const json::json_value* typeJsonValue = jsonObject->find(assetTypeField);
        if (typeJsonValue == nullptr)
        {
	        JUTILS_LOG(warning, JSTR("Can't find field \"assetType\" in asset file {}"), assetPath);
	        return false;
        }
        jstringID type = jstringID_NONE;
        {
	        jstring typeStr;
			if ((*typeJsonValue)->tryGetString(typeStr))
			{
				type = typeStr;
			}
        }
        if (type == assetTypeTexture)
        {
	        outAssetType = AssetType::Texture;
        }
        else
        {
	        JUTILS_LOG(warning, JSTR("Invalid value in field \"assetType\" in asset file {}"), assetPath);
            return false;
        }
        outConfig = config;
        return true;
    }
    
    bool ParseTextureAssetFile(const jstring& assetPath, const json::json_value& config, TextureAssetDescription& outDescription)
    {
        static const jstringID assetTextureField = JSTR("textureFile");
        static const jstringID assetTextureFormatField = JSTR("textureFormat");
        static const jstringID formatRGBA8 = JSTR("RGBA8");
        
        jstring jsonString;
        const jmap<jstringID, json::json_value>& jsonObject = config->asObject();
        const json::json_value* formatJsonValue = jsonObject.find(assetTextureFormatField);
        if ((formatJsonValue == nullptr) || !(*formatJsonValue)->tryGetString(jsonString))
        {
            JUTILS_LOG(warning, JSTR("Can't find field \"textureFormat\" in asset file {}"), assetPath);
	        return false;
        }
        JumaRE::TextureFormat format = JumaRE::TextureFormat::NONE;
        const jstringID formatStringID = jsonString;
        if (formatStringID == formatRGBA8)
        {
	        format = JumaRE::TextureFormat::RGBA8;
        }
        else
        {
	        JUTILS_LOG(warning, JSTR("Invalid value in field \"textureFormat\" in asset file {}"), assetPath);
            return false;
        }

        const json::json_value* textureJsonValue = jsonObject.find(assetTextureField);
        if ((textureJsonValue == nullptr) || !(*textureJsonValue)->tryGetString(jsonString))
        {
            JUTILS_LOG(warning, JSTR("Can't find field \"textureFile\" in asset file {}"), assetPath);
	        return false;
        }

        outDescription.textureDataPath = std::move(jsonString);
        outDescription.textureFormat = format;
        return true;
    }

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

        for (auto& material : m_Materials)
        {
            if (material.updatePtr() != nullptr)
            {
	            material->clearMaterial();
            }
        }
        m_Materials.clear();
        m_GlobalMaterialParams.clear();

        for (auto& shader : m_Shaders)
        {
            if (shader.value != nullptr)
            {
	            shader.value->clearShader();
            }
        }
        for (auto& shader : m_EngineShaders)
        {
            if (shader.value != nullptr)
            {
	            shader.value->clearShader();
            }
        }
        m_Shaders.clear();
        m_EngineShaders.clear();
        
        for (const auto& asset : m_LoadedAssets)
        {
	        asset.value->clearAsset();
        }
        m_LoadedAssets.clear();
	}
    
    const EngineObjectPtr<Shader>& AssetsEngineSubsystem::getEngineShader(const jstringID& shaderName)
	{
        return getShader(m_EngineShaders, shaderName, m_EngineContentDirectory);
	}
    const EngineObjectPtr<Shader>& AssetsEngineSubsystem::getShader(const jstringID& shaderName)
    {
        return getShader(m_Shaders, shaderName, m_GameContentDirectory);
    }
    const EngineObjectPtr<Shader>& AssetsEngineSubsystem::getShader(jmap<jstringID, EngineObjectPtr<Shader>>& shadersList, const jstringID& shaderName, const jstring& contentFolder) const
	{
        EngineObjectPtr<Shader>* shaderPtr = shadersList.find(shaderName);
        if (shaderPtr != nullptr)
        {
            return *shaderPtr;
        }

        EngineObjectPtr<Shader>& shader = shadersList.add(shaderName, getEngine()->createObject<Shader>());
        if (!shader->loadShader(shaderName, contentFolder))
        {
            JUTILS_LOG(error, JSTR("Failed to create shader {} from {}"), shaderName.toString(), contentFolder);
            shader = nullptr;
            return shader;
        }
        JUTILS_LOG(correct, JSTR("Created shader {} from {}"), shaderName.toString(), contentFolder);
        return shader;
	}

    EngineObjectPtr<Material> AssetsEngineSubsystem::createMaterial(const EngineObjectPtr<Shader>& shader)
    {
        EngineObjectPtr<Material> material = getEngine()->createObject<Material>();
        if (!material->createMaterial(shader))
        {
            JUTILS_LOG(error, JSTR("Failed to create material from shader {}"), shader != nullptr ? shader->getName().toString() : JSTR("NULL"));
            return nullptr;
        }
        m_Materials.add(material);
        return material;
    }
    EngineObjectPtr<Material> AssetsEngineSubsystem::createMaterial(const EngineObjectPtr<Material>& baseMaterial)
    {
        EngineObjectPtr<Material> material = getEngine()->createObject<Material>();
        if (!material->createMaterial(baseMaterial))
        {
            JUTILS_LOG(error, JSTR("Failed to create material instance"));
            return nullptr;
        }
        m_Materials.add(material);
        return material;
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

    EngineObjectPtr<Asset> AssetsEngineSubsystem::loadAsset(const jstringID& assetID, const AssetType expectedAssetType)
    {
        const jstring assetIDStr = assetID.toString();
        const EngineObjectPtr<Asset>* assetPtr = m_LoadedAssets.find(assetID);
        if (assetPtr != nullptr)
        {
            const AssetType type = (*assetPtr)->getAssetType();
            if (type != expectedAssetType)
            {
                JUTILS_LOG(warning, JSTR("Asset type {} of asset {} is not expected type {}"), type, assetIDStr, expectedAssetType);
	            return nullptr;
            }
	        return assetPtr->cast<Texture>();
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
        if (type != expectedAssetType)
        {
	        JUTILS_LOG(warning, JSTR("Failed to load asset {}"), assetIDStr);
	        return nullptr;
        }

        EngineObjectPtr<Asset> assetObject = nullptr;
        switch (type)
        {
        case AssetType::Texture:
	        {
		        TextureAssetDescription description;
		        if (!ParseTextureAssetFile(assetPath, config, description))
		        {
		            JUTILS_LOG(warning, JSTR("Failed parse asset {}"), assetIDStr);
			        return nullptr;
		        }

                EngineObjectPtr<Texture> texture = getEngine()->createObject<Texture>();
		        if ((texture == nullptr) || !texture->loadAsset(description))
		        {
			        JUTILS_LOG(error, JSTR("Failed to load texture asset {}"), assetIDStr);
		            return nullptr;
		        }
                assetObject = std::move(texture);
	        }
            break;
        default: ;
        }

        JUTILS_LOG(correct, JSTR("Loaded asset {} ({})"), assetIDStr, expectedAssetType);
        assetObject->m_AssetID = assetID;
        m_LoadedAssets.add(assetID, assetObject);
        return assetObject;
    }
    EngineObjectPtr<Texture> AssetsEngineSubsystem::getTextureAsset(const jstringID& assetID)
	{
        EngineObjectPtr<Asset> asset = loadAsset(assetID, AssetType::Texture);
        return asset != nullptr ? asset.castMove<Texture>() : nullptr;
	}
}
