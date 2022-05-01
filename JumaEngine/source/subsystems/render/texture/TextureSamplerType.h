// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "TextureFiltering.h"
#include "TextureWrapMode.h"

namespace JumaEngine
{
    struct TextureSamplerType
    {
        TextureFiltering filtering = TextureFiltering::Point;
        TextureWrapMode wrapMode = TextureWrapMode::Clamp;

        bool operator<(const TextureSamplerType& type) const { return (filtering < type.filtering) || ((filtering == type.filtering) && (wrapMode < type.wrapMode)); }
    };
}
