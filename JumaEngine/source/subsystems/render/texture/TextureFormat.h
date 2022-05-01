// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    enum class TextureFormat : uint8
    {
        None,
        RGBA_UINT8,
        BGRA_UINT8,
        DEPTH_FLOAT32,
        DEPTH_FLOAT32_STENCIL_UINT8,
        DEPTH_UNORM24_STENCIL_UINT8
    };

    inline uint32 GetTextureFormatSize(const TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::RGBA_UINT8:
        case TextureFormat::BGRA_UINT8:
        case TextureFormat::DEPTH_FLOAT32:
        case TextureFormat::DEPTH_UNORM24_STENCIL_UINT8:
            return 4;

        case TextureFormat::DEPTH_FLOAT32_STENCIL_UINT8: 
            return 5;

        default: ;
        }
        return 0;
    }
}
