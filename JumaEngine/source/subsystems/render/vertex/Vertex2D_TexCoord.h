// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "VertexInfo.h"
#include "jutils/math/vector2.h"

namespace JumaEngine
{
    struct Vertex2D_TexCoord
    {
        math::vector2 position;
        math::vector2 textureCoords;
    };

    template<>
    struct VertexInfo<Vertex2D_TexCoord> : std::true_type
    {
        JUMAENGINE_VERTEX_TYPE(Vertex2D_TexCoord)

        static jarray<VertexComponentDescription> getVertexComponents()
        {
            return {
                { 0, JSTR("position"), VertexComponentType::Vec2, offsetof(VertexType, position) },
                { 1, JSTR("textureCoords"), VertexComponentType::Vec2, offsetof(VertexType, textureCoords) }
            };
        }
    };
}
