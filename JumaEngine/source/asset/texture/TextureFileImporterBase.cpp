// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "TextureFileImporterBase.h"

namespace JumaEngine
{
    bool TextureFileImporterBase::importFile(const asset_ptr<TextureBase>& outTexture, const jstring& filePath)
    {
        if ((outTexture == nullptr) || outTexture->isInit() || filePath.isEmpty())
        {
            return false;
        }

        return importFileInternal(outTexture, filePath);
    }

    void TextureFileImporterBase::copyTextureData(const asset_ptr<TextureBase>& outTexture, const glm::uvec2& size, const TextureFormat format, const uint8* data)
    {
        outTexture->init(size, format, data);
    }
}
