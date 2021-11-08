// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    enum class ImageFormat
    {
        None,
        R8G8B8A8,
        B8G8R8A8,
        DEPTH_SFLOAT32,
        DEPTH_SFLOAT32_STENCIL_UINT8,
        DEPTH_UNORM24_STENCIL_UINT8
    };
}
