// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/Texture.h"

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

    bool Texture::loadAsset(const TextureAssetCreateInfo& createInfo)
    {
        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        if (renderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invaid render engine"));
            return false;
        }

        const AssetsEngineSubsystem* assetsSubsystem = getEngine()->getSubsystem<AssetsEngineSubsystem>();
        const jstring textureFilePath = assetsSubsystem->getAssetPath(createInfo.textureDataPath);

        math::ivector2 textureSize;
        int32 componentsCount = 0;
        stbi_uc* data = stbi_load(textureFilePath.getString(), &textureSize.x, &textureSize.y, &componentsCount, 4);
        if (data == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to load texture file {}"), textureFilePath);
            return false;
        }

        JumaRE::Texture* texture = renderEngine->createTexture(textureSize, createInfo.textureFormat, data);
        stbi_image_free(data);
        if (texture == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create texture from file {}"), textureFilePath);
            return false;
        }

        m_Texture = texture;
        return true;
    }

    void Texture::clearAsset()
    {
	    if (m_Texture != nullptr)
        {
            Super::clearAsset();

            getEngine()->getRenderEngine()->destroyTexture(m_Texture);
            m_Texture = nullptr;
        }
    }
}