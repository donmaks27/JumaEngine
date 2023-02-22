// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/Texture.h"

#include <jutils/configs/json_parser.h>
#include <jutils/math/vector3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "JumaEngine/Engine.h"

namespace JumaEngine
{
    JumaRE::TextureFormat ParseTextureFormatString(const jstring& format)
    {
        if (format == JSTR("RGBA8"))
        {
            return JumaRE::TextureFormat::RGBA8;
        }
        return JumaRE::TextureFormat::NONE;
    }
    
    bool Texture::loadTexture(const jstringID& textureName, const jstring& contentFolder)
    {
        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        if (renderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invaid render engine"));
            return false;
        }

        const jstring textureNameString = textureName.toString();
        const jstring configFilePath = contentFolder + JSTR("/textures/") + textureNameString + JSTR(".json");
        const json::json_value configJsonValue = json::parseFile(configFilePath);
        if (configJsonValue == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to parse texture config file {}"), configFilePath.getString());
            return false;
        }

        const jmap<jstringID, json::json_value>& configJsonObject = configJsonValue->asObject();
        const json::json_value* filePathJsonPtr = configJsonObject.find(JSTR("filePath"));
        if (filePathJsonPtr == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to parse field \"filePath\" from texture config file {}"), configFilePath.getString());
            return false;
        }
        const jstring& textureFilePath = contentFolder + (*filePathJsonPtr)->asString();

        const json::json_value* formatJsonPtr = configJsonObject.find(JSTR("format"));
        if (formatJsonPtr == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to parse field \"format\" from texture config file {}"), configFilePath.getString());
            return false;
        }
        const jstring& textureFormatString = (*formatJsonPtr)->asString();
        const JumaRE::TextureFormat textureFormat = ParseTextureFormatString(textureFormatString);
        if (textureFormat == JumaRE::TextureFormat::NONE)
        {
            JUTILS_LOG(error, JSTR("Failed to parse field \"format\" from texture config file {}"), configFilePath.getString());
            return false;
        }

        math::ivector2 textureSize;
        int32 componentsCount = 0;
        stbi_uc* data = stbi_load(textureFilePath.getString(), &textureSize.x, &textureSize.y, &componentsCount, 4);
        if (data == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to load texture file {}"), textureFilePath);
            return false;
        }

        JumaRE::Texture* texture = renderEngine->createTexture(textureSize, textureFormat, data);
        stbi_image_free(data);
        if (texture == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create texture {}"), textureNameString);
            return false;
        }

        m_Texture = texture;
        return true;
    }

    void Texture::clearTexture()
    {
        if (m_Texture != nullptr)
        {
            getEngine()->getRenderEngine()->destroyTexture(m_Texture);
            m_Texture = nullptr;
        }
    }
}