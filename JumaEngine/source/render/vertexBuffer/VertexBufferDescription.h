// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/jarray.h"

namespace JumaEngine
{
    enum class VertexComponentType
    {
        None,
        Float
    };

    struct VertexComponentDescription
    {
        int32 componentID = -1;
        VertexComponentType componentType = VertexComponentType::None;
        uint32 componentSize = 0;
        size_t componentOffset = 0;

        bool isValid() const { return (componentID >= 0) && (componentType != VertexComponentType::None) && (componentSize > 0); }
    };

    struct VertexBufferDescription
    {
        uint32 verticesCount = 0;
        uint32 indicesCount = 0;

        uint32 vertexSize = 0;
        jarray<VertexComponentDescription> vertexComponents;
    };
}
