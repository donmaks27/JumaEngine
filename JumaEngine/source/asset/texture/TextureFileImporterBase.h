// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "glm/vec2.hpp"
#include "render/texture/TextureBase.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class TextureFileImporterBase : public EngineContextObject
    {
        JUMAENGINE_CLASS(TextureFileImporterBase, EngineContextObject)

    public:
        TextureFileImporterBase() = default;
        virtual ~TextureFileImporterBase() override = default;

        bool importFile(TextureBase* outTexture, const jstring& filePath);

    protected:

        virtual bool importFileInternal(TextureBase* outTexture, const jstring& filePath) = 0;
        void copyTextureData(TextureBase* outTexture, const glm::uvec2& size, TextureFormat format, const uint8* data);
    };
}
