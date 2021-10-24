// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "glm/vec2.hpp"
#include "render/texture/TextureBase.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class TextureFileImporterBase : public EngineContextObjectOld
    {
        JUMAENGINE_CLASS_OLD(TextureFileImporterBase, EngineContextObjectOld)

    public:
        TextureFileImporterBase() = default;
        virtual ~TextureFileImporterBase() override = default;

        bool importFile(const asset_ptr<TextureBase>& outTexture, const jstring& filePath);

    protected:

        virtual bool importFileInternal(const asset_ptr<TextureBase>& outTexture, const jstring& filePath) = 0;
        void copyTextureData(const asset_ptr<TextureBase>& outTexture, const glm::uvec2& size, TextureFormat format, const uint8* data);
    };
}
