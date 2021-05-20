// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "asset/AssetObject.h"

namespace JumaEngine
{
    class TextureBase;

    struct TextureShaderUniform
    {
        asset_ptr<TextureBase> texture = nullptr;
        uint32 index = 0;
    };
}
