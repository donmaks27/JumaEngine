// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "AssetFilesParsing.h"

#include <jutils/configs/json_parser.h>

namespace JumaEngine
{
    bool LoadAssetFile(const jstring& assetPath, AssetType& outAssetType, json::json_value& outConfig)
    {
        static const jstringID assetTypeField = JSTR("assetType");
        static const jstringID assetTypeTexture = JSTR("texture");
        static const jstringID assetTypeRenderTarget = JSTR("renderTarget");
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
        else if (type == assetTypeRenderTarget) { outAssetType = AssetType::RenderTarget; }
        else if (type == assetTypeMaterial) { outAssetType = AssetType::Material; }
        else
        {
	        JUTILS_LOG(warning, JSTR("Invalid value in field \"assetType\" in asset file {}"), assetPath);
            return false;
        }
        outConfig = config;
        return true;
    }

    JumaRE::TextureFormat ParseAssetFile_TextureFormat(const jstring& format)
    {
        static const jstringID formatRGBA8 = JSTR("RGBA8");

        const jstringID formatStringID = format;
        if (formatStringID == formatRGBA8)
        {
	        return JumaRE::TextureFormat::RGBA8;
        }
        return JumaRE::TextureFormat::NONE;
    }
    bool ParseAssetFile_Vec2(const jarray<json::json_value>& jsonArray, math::vector2& outValue)
    {
        if (jsonArray.getSize() < 2) 
        {
            return false;
        }
        outValue = { static_cast<float>(jsonArray[0]->asNumber()), static_cast<float>(jsonArray[1]->asNumber()) };
        return true;
    }
    bool ParseAssetFile_Vec4(const jarray<json::json_value>& jsonArray, math::vector4& outValue)
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
    bool ParseAssetFile_Mat4(const jarray<json::json_value>& jsonArray, math::matrix4& outValue)
    {
        if (jsonArray.getSize() < 4) 
        {
            return false;
        }
        math::matrix4 value;
        if (!ParseAssetFile_Vec4(jsonArray[0]->asArray(), value[0]) || 
            !ParseAssetFile_Vec4(jsonArray[1]->asArray(), value[1]) || 
            !ParseAssetFile_Vec4(jsonArray[2]->asArray(), value[2]) || 
            !ParseAssetFile_Vec4(jsonArray[3]->asArray(), value[3]))
        {
            return false;
        }
        outValue = value;
        return true;
    }

    bool ParseTextureAssetFile(const jstring& assetPath, const json::json_value& config, TextureAssetCreateInfo& outCreateInfo)
    {
        static const jstringID assetTextureField = JSTR("textureFile");
        static const jstringID assetTextureFormatField = JSTR("textureFormat");
        
        jstring jsonString;
        const jmap<jstringID, json::json_value>& jsonObject = config->asObject();
        const json::json_value* formatJsonValue = jsonObject.find(assetTextureFormatField);
        if ((formatJsonValue == nullptr) || !(*formatJsonValue)->tryGetString(jsonString))
        {
            JUTILS_LOG(warning, JSTR("Can't find field \"textureFormat\" in asset file {}"), assetPath);
	        return false;
        }
        const JumaRE::TextureFormat format = ParseAssetFile_TextureFormat(jsonString);
        if (format == JumaRE::TextureFormat::NONE)
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

    bool ParseRenderTargetAssetFile(const jstring &assetPath, const json::json_value &config, RenderTargetCreateInfo &outCreateInfo)
    {
        static const jstringID fieldFormat = JSTR("format");
        static const jstringID fieldSamples = JSTR("samples");
        static const jstringID fieldSize = JSTR("size");

        jstring jsonString;
        const jmap<jstringID, json::json_value>& jsonObject = config->asObject();
        const json::json_value* formatJsonValue = jsonObject.find(fieldFormat);
        if ((formatJsonValue == nullptr) || !(*formatJsonValue)->tryGetString(jsonString))
        {
            JUTILS_LOG(warning, JSTR("Can't find field \"format\" in asset file {}"), assetPath);
	        return false;
        }
        const JumaRE::TextureFormat format = ParseAssetFile_TextureFormat(jsonString);
        if (format == JumaRE::TextureFormat::NONE)
        {
	        JUTILS_LOG(warning, JSTR("Invalid value in field \"format\" in asset file {}"), assetPath);
            return false;
        }

        int32 samplesNumber = 0;
        const json::json_value* samplesJsonValue = jsonObject.find(fieldSamples);
        if ((samplesJsonValue == nullptr) || !(*samplesJsonValue)->tryGetNumber(samplesNumber))
        {
            JUTILS_LOG(warning, JSTR("Can't find field \"samples\" in asset file {}"), assetPath);
            return false;
        }
        JumaRE::TextureSamples samples;
        switch (samplesNumber)
        {
        case 1:  samples = JumaRE::TextureSamples::X1;  break;
        case 2:  samples = JumaRE::TextureSamples::X2;  break;
        case 4:  samples = JumaRE::TextureSamples::X4;  break;
        case 8:  samples = JumaRE::TextureSamples::X8;  break;
        case 16: samples = JumaRE::TextureSamples::X16; break;
        default: 
            JUTILS_LOG(warning, JSTR("Invalid value in field \"samples\" in asset file {}"), assetPath);
            return false;
        }

        const json::json_value* sizeJsonValue = jsonObject.find(fieldSize);
        if (sizeJsonValue == nullptr)
        {
            JUTILS_LOG(warning, JSTR("Can't find field \"size\" in asset file {}"), assetPath);
            return false;
        }
        math::vector2 size;
        if (!ParseAssetFile_Vec2((*sizeJsonValue)->asArray(), size))
        {
            JUTILS_LOG(warning, JSTR("Invalid value in field \"size\" in asset file {}"), assetPath);
            return false;
        }

        outCreateInfo.format = format;
        outCreateInfo.samples = samples;
        outCreateInfo.size = size;
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
                        if (ParseAssetFile_Vec2((*defaultValue)->asArray(), value))
                        {
                            outDefaultValues.values_vec2.add(paramNameID, value);
                        }
                    }
                    break;
                case JumaRE::ShaderUniformType::Vec4: 
                    {
                        math::vector4 value;
                        if (ParseAssetFile_Vec4((*defaultValue)->asArray(), value))
                        {
                            outDefaultValues.values_vec4.add(paramNameID, value);
                        }
                    }
                    break;
                case JumaRE::ShaderUniformType::Mat4: 
                    {
                        math::matrix4 value;
                        if (ParseAssetFile_Mat4((*defaultValue)->asArray(), value))
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

    bool ParseMaterialInstanceAssetFile(const jstring &assetPath, const json::json_value &config, const EngineObjectPtr<Material>& parentMaterial, 
        MaterialInstanceCreateInfo &outCreateInfo)
    {
        static const jstringID fieldParams = JSTR("materialParams");

        JumaRE::Shader* shader = parentMaterial != nullptr ? parentMaterial->getShader() : nullptr;
        if (shader == nullptr)
        {
            return false;
        }

        outCreateInfo.parentMaterial = parentMaterial;

        const json::json_value* overridedParamsValue = config->asObject().find(fieldParams);
        if (overridedParamsValue != nullptr)
        {
            const jmap<jstringID, JumaRE::ShaderUniform>& uniforms = shader->getUniforms();
            for (const auto& overridedParamValue : (*overridedParamsValue)->asObject())
            {
                jstringID uniformName;
                if (!parentMaterial->getUniformNameForParam(overridedParamValue.key, uniformName))
                {
                    continue;
                }
                const JumaRE::ShaderUniform* uniform = uniforms.find(uniformName);
                if (uniform == nullptr)
                {
                    continue;
                }
                switch (uniform->type)
                {
                case JumaRE::ShaderUniformType::Float:
                    {
                        double value;
                        if (overridedParamValue.value->tryGetNumber(value))
                        {
                            outCreateInfo.overridedParams.values_float.add(overridedParamValue.key, static_cast<float>(value));
                        }
                    }
                    break;
                case JumaRE::ShaderUniformType::Vec2:
                    {
                        math::vector2 value;
                        if (ParseAssetFile_Vec2(overridedParamValue.value->asArray(), value))
                        {
                            outCreateInfo.overridedParams.values_vec2.add(overridedParamValue.key, value);
                        }
                    }
                    break;
                case JumaRE::ShaderUniformType::Vec4:
                    {
                        math::vector4 value;
                        if (ParseAssetFile_Vec4(overridedParamValue.value->asArray(), value))
                        {
                            outCreateInfo.overridedParams.values_vec4.add(overridedParamValue.key, value);
                        }
                    }
                    break;
                case JumaRE::ShaderUniformType::Mat4:
                    {
                        math::matrix4 value;
                        if (ParseAssetFile_Mat4(overridedParamValue.value->asArray(), value))
                        {
                            outCreateInfo.overridedParams.values_mat4.add(overridedParamValue.key, value);
                        }
                    }
                    break;
                case JumaRE::ShaderUniformType::Texture:
                    {
                        jstring value;
                        if (overridedParamValue.value->tryGetString(value))
                        {
                            outCreateInfo.overridedParams.values_texture.add(overridedParamValue.key, value);
                        }
                    }
                    break;
                
                default:
                    break;
                }
            }
        }
        return true;
    }
}