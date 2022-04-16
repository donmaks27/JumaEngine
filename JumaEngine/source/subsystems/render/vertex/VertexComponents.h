// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "jutils/jarray.h"

namespace JumaEngine
{
    enum class VertexComponentType
    {
        None,
        Float,
        Vec2,
        Vec3,
        Vec4
    };

    struct VertexComponentDescription
    {
        int32 ID = -1;
        VertexComponentType type = VertexComponentType::None;
        uint32 offset = 0;
    };

    struct VertexDescription
    {
        uint32 size = 0;
        jarray<VertexComponentDescription> components;
    };
}
