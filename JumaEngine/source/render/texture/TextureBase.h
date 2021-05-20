// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "glm/vec2.hpp"
#include "asset/AssetObject.h"

namespace JumaEngine
{
    class TextureFileImporterBase;

    enum class TextureFormat
    {
        None,
        R8G8B8,
        R8G8B8A8,
        B8G8R8,
        B8G8R8A8,
        R8
    };

    class TextureBase : public AssetObject
    {
        JUMAENGINE_CLASS(TextureBase, AssetObject)

    public:
        TextureBase() = default;
        virtual ~TextureBase() override = default;
        
        bool init(const glm::uvec2& size, TextureFormat format, const uint8* data);
        bool isInit() const { return m_Initialized; }

        virtual void activate(uint32 index) = 0;
        virtual void deactivate(uint32 index) {}

    protected:

        bool m_Initialized = false;

        glm::uvec2 m_Size = { 0, 0 };
        TextureFormat m_Format = TextureFormat::None;


        virtual bool initInternal(const uint8* data) = 0;
    };
}
