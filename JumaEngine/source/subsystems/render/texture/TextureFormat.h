// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    enum class TextureFormat : uint8
    {
        None,
        RGBA,
        BGRA,
        SFLOAT32,
        SFLOAT32_UINT8,
        UNORM24_UINT8
    };

    inline uint32 GetTextureFormatSize(const TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::RGBA:
        case TextureFormat::BGRA:
        case TextureFormat::SFLOAT32:
        case TextureFormat::UNORM24_UINT8:
            return 4;

        case TextureFormat::SFLOAT32_UINT8: 
            return 5;

        default: ;
        }
        return 0;
    }
}
