// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "TextureFileImporterBase.h"

namespace JumaEngine
{
    class TextureFileImporter_DevIL : public TextureFileImporterBase
    {
        JUMAENGINE_CLASS(TextureFileImporter_DevIL, TextureFileImporterBase)

    public:
        TextureFileImporter_DevIL() = default;
        virtual ~TextureFileImporter_DevIL() override = default;

    protected:

        virtual void onRegister() override;

        virtual bool importFileInternal(const asset_ptr<TextureBase>& outTexture, const jstring& filePath) override;
    };
}
