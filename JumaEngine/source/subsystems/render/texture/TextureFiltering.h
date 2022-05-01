// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    enum class TextureFiltering : uint8
    {
        Point,
        Bilinear,
        Trilinear,
        Anisotropic_2,
        Anisotropic_4,
        Anisotropic_8,
        Anisotropic_16
    };
}
