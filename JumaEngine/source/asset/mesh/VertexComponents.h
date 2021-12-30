// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/jarray.h"

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
        int32 componentID = -1;
        VertexComponentType componentType = VertexComponentType::None;
        size_t componentOffset = 0;

        bool isValid() const { return (componentID >= 0) && (componentType != VertexComponentType::None); }
    };
}
