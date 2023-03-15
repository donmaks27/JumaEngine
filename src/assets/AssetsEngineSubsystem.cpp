// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/AssetsEngineSubsystem.h"

#include <jutils/configs/json_parser.h>

#include "JumaEngine/assets/MaterialBase.h"
#include "JumaEngine/assets/MaterialInstance.h"
#include "JumaEngine/engine/ConfigEngineSubsystem.h"
#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
    bool LoadAssetFile(const jstring& assetPath, AssetType& outAssetType, json::json_value& outConfig)
    {
        static const jstringID assetTypeField = JSTR("assetType");
        static const jstringID assetTypeTexture = JSTR("texture");
        static const jstringID assetTypeMaterial = JSTR("material");

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
        if (type == assetTypeTexture) { outAssetType = AssetType::Texture; }
        else if (type == assetTypeMaterial) { outAssetType = AssetType::Material; }
        else
        {
	        JUTILS_LOG(warning, JSTR("Invalid value in field \"assetType\" in asset file {}"), assetPath);
            return false;
        }
        outConfig = config;
        return true;
    }
    
    bool ParseTextureAssetFile(const jstring& assetPath, const json::json_value& config, TextureAssetCreateInfo& outCreateInfo)
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
        JumaRE::TextureFormat format;
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

        outCreateInfo.textureDataPath = std::move(jsonString);
        outCreateInfo.textureFormat = format;
        return true;
    }

    bool ParseMaterialAssetFile_ShaderFiles(const jstring& assetPath, const jmap<jstringID, json::json_value>& jsonObject, 
        const JumaRE::RenderAPI renderAPI, jmap<JumaRE::ShaderStageFlags, jstring>& outShaderFiles)
    {
	    static const jstringID assetShadersField = JSTR("shaderFiles");
        static const jstringID shaderVertex = JSTR("vertex");
        static const jstringID shaderFragment = JSTR("fragment");

        const json::json_value* shadersValue = jsonObject.find(assetShadersField);
        const jmap<jstringID, json::json_value>* shadersObject = nullptr;
        if ((shadersValue == nullptr) || !(*shadersValue)->tryGetObject(shadersObject))
        {
	        JUTILS_LOG(warning, JSTR("Can't get field \"shaderFiles\" in asset file {}"), assetPath);
	        return false;
        }
        jmap<JumaRE::ShaderStageFlags, jstring> shaderFiles;
        for (const auto& shaderValue : *shadersObject)
        {
            JumaRE::ShaderStageFlags stage;
	        if (shaderValue.key == shaderVertex)        { stage = JumaRE::ShaderStageFlags::SHADER_STAGE_VERTEX; }
            else if (shaderValue.key == shaderFragment) { stage = JumaRE::ShaderStageFlags::SHADER_STAGE_FRAGMENT; }
            else { continue; }

            const jmap<jstringID, json::json_value>* shaderFilesObject = nullptr;
            if ((shaderValue.value == nullptr) || !shaderValue.value->tryGetObject(shaderFilesObject))
            {
	            continue;
            }
            jstring shaderFile;
            const json::json_value* shaderFileValue = shaderFilesObject->find(JumaRE::RenderAPIToString(renderAPI));
            if ((shaderFileValue == nullptr) || !(*shaderFileValue)->tryGetString(shaderFile))
            {
	            continue;
            }

            shaderFiles.add(stage, shaderFile);
        }
        if (!shaderFiles.contains(JumaRE::ShaderStageFlags::SHADER_STAGE_VERTEX) || !shaderFiles.contains(JumaRE::ShaderStageFlags::SHADER_STAGE_FRAGMENT))
        {
	        JUTILS_LOG(warning, JSTR("Failed parse material asset file {}({}): there should be at least vertex AND fragment shader files"), assetPath, JumaRE::RenderAPIToString(renderAPI));
	        return false;
        }

        outShaderFiles = std::move(shaderFiles);
        return true;
    }
    bool ParseMaterialAssetFile_VertexComponents(const jstring& assetPath, const jmap<jstringID, json::json_value>& jsonObject, 
        jset<jstringID>& outComponents)
    {
	    static const jstringID assetComponenentsField = JSTR("vertexComponents");

	    const json::json_value* vertexComponentsValue = jsonObject.find(assetComponenentsField);
        if (vertexComponentsValue == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to find \"vertexComponents\" field in asset file {}"), assetPath);
            return false;
        }
        jset<jstringID> vertexComponents;
        for (const auto& vertexComponentValue : (*vertexComponentsValue)->asArray())
        {
	        jstring vertexComponent;
            if ((vertexComponentValue != nullptr) && vertexComponentValue->tryGetString(vertexComponent))
            {
                vertexComponents.add(vertexComponent);
            }
        }
        if (vertexComponents.isEmpty())
        {
	        JUTILS_LOG(error, JSTR("Failed parse material asset file {}: there should be at least one vertex component"), assetPath);
            return false;
        }

        outComponents = std::move(vertexComponents);
        return true;
    }
    jmap<jstringID, JumaRE::ShaderUniform> ParseMaterialAssetFile_Uniforms(const jmap<jstringID, json::json_value>& jsonObject)
    {
	    static const jstringID assetUniformsField = JSTR("uniforms");
	    static const jstringID typeStringID = JSTR("type");
	    static const jstringID stagesStringID = JSTR("stages");
	    static const jstringID shaderLocationStringID = JSTR("shaderLocation");
	    static const jstringID shaderOffsetStringID = JSTR("shaderBlockOffset");

        const json::json_value* uniformsValue = jsonObject.find(assetUniformsField);
        const jmap<jstringID, json::json_value>* uniformsObject = nullptr;
        if ((uniformsValue == nullptr) || !(*uniformsValue)->tryGetObject(uniformsObject))
        {
	        return {};
        }
        jmap<jstringID, JumaRE::ShaderUniform> uniforms;
        for (const auto& uniform : *uniformsObject)
        {
	        const jmap<jstringID, json::json_value>* uniformObject = nullptr;
            if ((uniform.value == nullptr) || !uniform.value->tryGetObject(uniformObject))
            {
	            continue;
            }

            jstring uniformTypeString;
            const json::json_value* typeValue = uniformObject->find(typeStringID);
            if ((typeValue == nullptr) || !(*typeValue)->tryGetString(uniformTypeString))
            {
	            continue;
            }
            JumaRE::ShaderUniformType uniformType;
            if (uniformTypeString == JSTR("float")) { uniformType = JumaRE::ShaderUniformType::Float; }
            else if (uniformTypeString == JSTR("vec2")) { uniformType = JumaRE::ShaderUniformType::Vec2; }
            else if (uniformTypeString == JSTR("vec4")) { uniformType = JumaRE::ShaderUniformType::Vec4; }
            else if (uniformTypeString == JSTR("mat4")) { uniformType = JumaRE::ShaderUniformType::Mat4; }
            else if (uniformTypeString == JSTR("texture")) { uniformType = JumaRE::ShaderUniformType::Texture; }
            else { continue; }

            const jarray<json::json_value>* stagesArray = nullptr;
            const json::json_value* stagesValue = uniformObject->find(stagesStringID);
            if ((stagesValue == nullptr) || !(*stagesValue)->tryGetArray(stagesArray))
            {
	            continue;
            }
            uint8 uniformStages = 0;
            for (const auto& stageValue : *stagesArray)
            {
	            jstring uniformStageString;
                if ((stageValue == nullptr) || !stageValue->tryGetString(uniformStageString))
                {
	                continue;
                }
                if (uniformStageString == JSTR("vertex"))
                {
	                uniformStages |= JumaRE::SHADER_STAGE_VERTEX;
                }
                else if (uniformStageString == JSTR("fragment"))
                {
	                uniformStages |= JumaRE::SHADER_STAGE_FRAGMENT;
                }
            }

            uint32 uniformLocation = 0;
            const json::json_value* uniformLocationValue = uniformObject->find(shaderLocationStringID);
            if ((uniformLocationValue == nullptr) || !(*uniformLocationValue)->tryGetNumber(uniformLocation))
            {
	            continue;
            }

            uint32 uniformBlockOffset = 0;
            const json::json_value* uniformBlockOffsetValue = uniformObject->find(shaderOffsetStringID);
            if ((uniformBlockOffsetValue == nullptr) || !(*uniformBlockOffsetValue)->tryGetNumber(uniformBlockOffset))
            {
	            continue;
            }

            uniforms.add(uniform.key, { uniformType, uniformStages, uniformLocation, uniformBlockOffset });
        }
        return uniforms;
    }
    bool ParseMaterialAssetFile_ValueVec2(const jarray<json::json_value>& jsonArray, math::vector2& outValue)
    {
        if (jsonArray.getSize() < 2) 
        {
            return false;
        }
        outValue = { static_cast<float>(jsonArray[0]->asNumber()), static_cast<float>(jsonArray[1]->asNumber()) };
        return true;
    }
    bool ParseMaterialAssetFile_ValueVec4(const jarray<json::json_value>& jsonArray, math::vector4& outValue)
    {
        if (jsonArray.getSize() < 4) 
        {
            return false;
        }
        outValue = { 
            static_cast<float>(jsonArray[0]->asNumber()), static_cast<float>(jsonArray[1]->asNumber()), 
            static_cast<float>(jsonArray[2]->asNumber()), static_cast<float>(jsonArray[3]->asNumber())
        };
        return true;
    }
    bool ParseMaterialAssetFile_ValueMat4(const jarray<json::json_value>& jsonArray, math::matrix4& outValue)
    {
        if (jsonArray.getSize() < 4) 
        {
            return false;
        }
        math::matrix4 value;
        if (!ParseMaterialAssetFile_ValueVec4(jsonArray[0]->asArray(), value[0]) || 
            !ParseMaterialAssetFile_ValueVec4(jsonArray[1]->asArray(), value[1]) || 
            !ParseMaterialAssetFile_ValueVec4(jsonArray[2]->asArray(), value[2]) || 
            !ParseMaterialAssetFile_ValueVec4(jsonArray[3]->asArray(), value[3]))
        {
            return false;
        }
        outValue = value;
        return true;
    }
    void ParseMaterialAssetFile_Params(const jmap<jstringID, json::json_value>& jsonObject, 
        const jmap<jstringID, JumaRE::ShaderUniform>& uniforms, jarray<MaterialParamCreateInfo>& outParams, 
        MaterialDefaultParamValues& outDefaultValues)
    {
        static const jstringID fieldMaterialParams = JSTR("materialParams");
        static const jstringID fieldUniform = JSTR("uniform");
        static const jstringID fieldName = JSTR("name");
        static const jstringID fieldDefaultValue = JSTR("defaultValue");

        const json::json_value* paramsValue = jsonObject.find(fieldMaterialParams);
        if (paramsValue == nullptr)
        {
            return;
        }
        jset<jstringID> addedUniforms, addedParams;
        for (const auto& paramValue : (*paramsValue)->asArray())
        {
            if (paramValue == nullptr)
            {
                continue;
            }
            const jmap<jstringID, json::json_value>& paramObject = paramValue->asObject();

            jstring uniformName, paramName;
            const json::json_value* uniformValue = paramObject.find(fieldUniform);
            const json::json_value* nameValue = paramObject.find(fieldName);
            if ((uniformValue == nullptr) || (nameValue == nullptr) || 
                !(*uniformValue)->tryGetString(uniformName) || !(*nameValue)->tryGetString(paramName))
            {
                continue;
            }
            jstringID uniformNameID = uniformName, paramNameID = paramName;
            if (addedUniforms.contains(uniformNameID) || addedParams.contains(paramNameID))
            {
                continue;
            }
            const JumaRE::ShaderUniform* uniform = uniforms.find(uniformNameID);
            if (uniform == nullptr)
            {
                continue;
            }
            addedUniforms.add(uniformNameID); addedParams.add(paramNameID);
            outParams.add({ paramNameID, uniformNameID });

            const json::json_value* defaultValue = paramObject.find(fieldDefaultValue);
            if (defaultValue != nullptr)
            {
                switch (uniform->type)
                {
                case JumaRE::ShaderUniformType::Float: 
                    {
                        double value = 0.0;
                        if ((*defaultValue)->tryGetNumber(value))
                        {
                            outDefaultValues.values_float.add(paramNameID, static_cast<float>(value));
                        }
                    }
                    break;
                case JumaRE::ShaderUniformType::Vec2: 
                    {
                        math::vector2 value;
                        if (ParseMaterialAssetFile_ValueVec2((*defaultValue)->asArray(), value))
                        {
                            outDefaultValues.values_vec2.add(paramNameID, value);
                        }
                    }
                    break;
                case JumaRE::ShaderUniformType::Vec4: 
                    {
                        math::vector4 value;
                        if (ParseMaterialAssetFile_ValueVec4((*defaultValue)->asArray(), value))
                        {
                            outDefaultValues.values_vec4.add(paramNameID, value);
                        }
                    }
                    break;
                case JumaRE::ShaderUniformType::Mat4: 
                    {
                        math::matrix4 value;
                        if (ParseMaterialAssetFile_ValueMat4((*defaultValue)->asArray(), value))
                        {
                            outDefaultValues.values_mat4.add(paramNameID, value);
                        }
                    }
                    break;
                case JumaRE::ShaderUniformType::Texture: 
                    {
                        jstring value;
                        if ((*defaultValue)->tryGetString(value))
                        {
                            outDefaultValues.values_texture.add(paramNameID, value);
                        }
                    }
                    break;
                default: ;
                }
            }
        }
    }
    bool ParseMaterialAssetFile(const jstring& assetPath, const json::json_value& config, const JumaRE::RenderAPI renderAPI, 
        MaterialBaseCreateInfo& outCreateInfo)
    {
        const jmap<jstringID, json::json_value>& jsonObject = config->asObject();
        jmap<JumaRE::ShaderStageFlags, jstring> shaderFiles;
        if (!ParseMaterialAssetFile_ShaderFiles(assetPath, jsonObject, renderAPI, shaderFiles))
        {
	        return false;
        }
        jset<jstringID> vertexComponents;
        if (!ParseMaterialAssetFile_VertexComponents(assetPath, jsonObject, vertexComponents))
        {
	        return false;
        }
        outCreateInfo = { std::move(shaderFiles), std::move(vertexComponents), ParseMaterialAssetFile_Uniforms(jsonObject) };
        ParseMaterialAssetFile_Params(jsonObject, outCreateInfo.shaderUniforms, outCreateInfo.params, outCreateInfo.defaultValues);
        return true;
    }
    /*bool ParseMaterialInstanceAssetFile(const jstring& assetPath, const json::json_value& config, jstringID& outParentAssetID, 
        MaterialInstanceCreateInfo& outCreateInfo)
    {
	    
    }*/

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
        case AssetType::Material:
	        {
                static const jstringID parentStringID = JSTR("parent");
		        if (config->asObject().contains(parentStringID))
		        {
			        return nullptr;
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
    EngineObjectPtr<Material> AssetsEngineSubsystem::getMaterialAsset(const jstringID& assetID)
    {
        EngineObjectPtr<Asset> asset = loadAsset(assetID, AssetType::Material);
        return asset != nullptr ? asset.castMove<Material>() : nullptr;
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
