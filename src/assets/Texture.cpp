// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/Texture.h"

#include <jutils/configs/json_parser.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "JumaEngine/assets/AssetsEngineSubsystem.h"
#include "JumaEngine/engine/Engine.h"

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

    bool Texture::loadAsset(const TextureAssetDescription& description)
    {
        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        if (renderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invaid render engine"));
            return false;
        }

        const AssetsEngineSubsystem* assetsSubsystem = getEngine()->getSubsystem<AssetsEngineSubsystem>();
        const jstring textureFilePath = assetsSubsystem->getAssetPath(description.textureDataPath);

        math::ivector2 textureSize;
        int32 componentsCount = 0;
        stbi_uc* data = stbi_load(textureFilePath.getString(), &textureSize.x, &textureSize.y, &componentsCount, 4);
        if (data == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to load texture file {}"), textureFilePath);
            return false;
        }

        JumaRE::Texture* texture = renderEngine->createTexture(textureSize, description.textureFormat, data);
        stbi_image_free(data);
        if (texture == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create texture from file {}"), textureFilePath);
            return false;
        }

        m_Texture = texture;
        return true;
    }

    void Texture::onObjectDescriptorDestroying()
    {
        onDestroying.call(this);
        clearTexture();
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