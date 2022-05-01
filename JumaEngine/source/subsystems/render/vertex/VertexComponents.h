// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "jutils/jstringID.h"
#include "jutils/jarray.h"

namespace JumaEngine
{
    enum class VertexComponentType
    {
        Float,
        Vec2,
        Vec3,
        Vec4
    };

    struct VertexComponentDescription
    {
        uint32 location = 0;
        jstringID name = jstringID_NONE;
        VertexComponentType type = VertexComponentType::Float;
        uint32 offset = 0;
    };

    struct VertexDescription
    {
        uint32 size = 0;
        jarray<VertexComponentDescription> components;
    };
}
