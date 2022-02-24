// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "jutils/jarray.h"
#include "jutils/math/vector2.h"
#include "jutils/math/vector3.h"

namespace JumaEngine
{
    struct ImportedVertex
    {
        math::vector3 position = math::vector3(0);
        math::vector3 normal = math::vector3(0);
        math::vector2 textureCoords = math::vector2(0);
    };

    struct ImportedVertexBuffer
    {
        jarray<ImportedVertex> vertices;
        jarray<uint32> vertexIndices;
    };
}
