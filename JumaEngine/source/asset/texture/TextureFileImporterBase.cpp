// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "TextureFileImporterBase.h"

namespace JumaEngine
{
    bool TextureFileImporterBase::importFile(TextureBase* outTexture, const jstring& filePath)
    {
        if ((outTexture == nullptr) || outTexture->isInit() || filePath.empty())
        {
            return false;
        }

        return importFileInternal(outTexture, filePath);
    }

    void TextureFileImporterBase::copyTextureData(TextureBase* outTexture, const glm::uvec2& size, const TextureFormat format, const uint8* data)
    {
        outTexture->initTexture(size, format, data);
    }
}
