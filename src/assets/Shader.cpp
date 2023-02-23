// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/Shader.h"

#include <jutils/configs/json_parser.h>

#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
    inline bool ParseShaderStage(const jstringID& str, JumaRE::ShaderStageFlags& outStage)
    {
        if (str == JSTR("vertex"))
        {
            outStage = JumaRE::SHADER_STAGE_VERTEX;
            return true;
        }
        if (str == JSTR("fragment"))
        {
            outStage = JumaRE::SHADER_STAGE_FRAGMENT;
            return true;
        }
        return false;
    }
    inline bool ParseShaderFiles(const JumaRE::RenderEngine* renderEngine, const jmap<jstringID, json::json_value>& jsonObject, 
        const jstring& contentFolder, jmap<JumaRE::ShaderStageFlags, jstring>& outFiles)
    {
        const json::json_value* jsonValue = jsonObject.find("shaderFiles");
        if (jsonValue == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to find \"shaderFiles\" field"));
            return false;
        }
        outFiles.clear();
        const jstring renderAPI = JumaRE::RenderAPIToString(renderEngine->getRenderAPI());
        for (const auto& shaderStage : (*jsonValue)->asObject())
        {
            JumaRE::ShaderStageFlags stage;
            if ((shaderStage.value == nullptr) || !ParseShaderStage(shaderStage.key, stage))
            {
                continue;
            }

            jstring shaderFilePath;
            const json::json_value* shaderFilePathJsonValue = shaderStage.value->asObject().find(renderAPI);
            if ((shaderFilePathJsonValue != nullptr) && (*shaderFilePathJsonValue)->tryGetString(shaderFilePath))
            {
                outFiles.add(stage, contentFolder + shaderFilePath);
            }
        }
        return true;
    }
    inline bool ParseVertexComponents(const jmap<jstringID, json::json_value>& jsonObject, jset<jstringID>& outVertexComponents)
    {
        const json::json_value* vertexComponentsJsonValue = jsonObject.find(JSTR("vertexComponents"));
        if (vertexComponentsJsonValue == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to fine \"vertexComponents\" field"));
            return false;
        }
        outVertexComponents.clear();
        for (const auto& vertexComponentJsonValue : (*vertexComponentsJsonValue)->asArray())
        {
            jstring vertexComponent;
            if ((vertexComponentJsonValue != nullptr) && vertexComponentJsonValue->tryGetString(vertexComponent))
            {
                outVertexComponents.add(vertexComponent);
            }
        }
        return !outVertexComponents.isEmpty();
    }
    inline bool ParseVertexUniforms(const jmap<jstringID, json::json_value>& jsonObject, jmap<jstringID, JumaRE::ShaderUniform>& outUniforms, 
        jmap<jstringID, jstringID>& outEngineInternalParams)
    {
        const json::json_value* uniformsJsonValue = jsonObject.find(JSTR("uniforms"));
        if (uniformsJsonValue == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to fine \"uniforms\" field"));
            return false;
        }
        outUniforms.clear();
        for (const auto& uniformJson : (*uniformsJsonValue)->asObject())
        {
            if (uniformJson.value == nullptr)
            {
                continue;
            }
            const jmap<jstringID, json::json_value>& uniformJsonObject = uniformJson.value->asObject();

            jstring uniformTypeString;
            const json::json_value* uniformTypeJsonValue = uniformJsonObject.find(JSTR("type"));
            if ((uniformTypeJsonValue == nullptr) || !(*uniformTypeJsonValue)->tryGetString(uniformTypeString))
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

            const json::json_value* uniformStagesJsonValue = uniformJsonObject.find(JSTR("stages"));
            if (uniformStagesJsonValue == nullptr)
            {
                continue;
            }
            uint8 uniformStages = 0;
            for (const auto& uniformStageJsonValue : (*uniformStagesJsonValue)->asArray())
            {
                jstring uniformStageString;
                if ((uniformStageJsonValue != nullptr) && uniformStageJsonValue->tryGetString(uniformStageString))
                {
                    JumaRE::ShaderStageFlags uniformStage;
                    if (ParseShaderStage(uniformStageString, uniformStage))
                    {
                        uniformStages |= uniformStage;
                    }
                }
            }

            uint32 uniformLocation = 0;
            const json::json_value* uniformLocationJsonValue = uniformJsonObject.find(JSTR("shaderLocation"));
            if (uniformLocationJsonValue != nullptr)
            {
                (*uniformLocationJsonValue)->tryGetNumber(uniformLocation);
            }
            
            uint32 uniformBlockOffset = 0;
            const json::json_value* uniformBlockOffsetJsonValue = uniformJsonObject.find(JSTR("shaderBlockOffset"));
            if (uniformBlockOffsetJsonValue != nullptr)
            {
                (*uniformBlockOffsetJsonValue)->tryGetNumber(uniformBlockOffset);
            }

            jstring engineInternalParamString;
            const json::json_value* engineInternalParamValue = uniformJsonObject.find(JSTR("engineInternalParam"));
            if ((engineInternalParamValue != nullptr) && (*engineInternalParamValue)->tryGetString(engineInternalParamString))
            {
                outEngineInternalParams.add(uniformJson.key, engineInternalParamString);
            }

            outUniforms.add(uniformJson.key, { uniformType, uniformStages, uniformLocation, uniformBlockOffset });
        }
        return true;
    }

    bool Shader::loadShader(const jstringID& shaderName, const jstring& contentFolder)
    {
        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        if (renderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invalid render engine"));
            return false;
        }

        const jstring shaderNameString = shaderName.toString();
        const jstring jsonFileName = contentFolder + JSTR("shaders/") + shaderNameString + JSTR(".json");
        const json::json_value shaderJsonValue = json::parseFile(jsonFileName);
        if (shaderJsonValue == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to parse file {}"), jsonFileName);
            return false;
        }
        const jmap<jstringID, json::json_value>& shaderJsonObject = shaderJsonValue->asObject();

        jmap<JumaRE::ShaderStageFlags, jstring> shaderFiles;
        jset<jstringID> vertexComponents;
        jmap<jstringID, JumaRE::ShaderUniform> uniforms;
        jmap<jstringID, jstringID> engineInternalParams;
        if (!ParseShaderFiles(renderEngine, shaderJsonObject, contentFolder, shaderFiles) || 
            !ParseVertexComponents(shaderJsonObject, vertexComponents) ||
            !ParseVertexUniforms(shaderJsonObject, uniforms, engineInternalParams))
        {
            JUTILS_LOG(error, JSTR("Failed to parse shader JSON file {}"), jsonFileName);
            return false;
        }

        JumaRE::Shader* shader = renderEngine->createShader(shaderFiles, std::move(vertexComponents), std::move(uniforms));
        if (shader == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create shader {}"), shaderNameString);
            return false;
        }

        m_Shader = shader;
        m_ShaderName = shaderName;
        m_EngineInternalParamNames = engineInternalParams;
        return true;
    }

    void Shader::clearShader()
    {
        if (m_Shader != nullptr)
        {
            getEngine()->getRenderEngine()->destroyShader(m_Shader);
            m_Shader = nullptr;
        }
    }
}
