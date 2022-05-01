// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "VertexInfo.h"
#include "jutils/math/vector2.h"

namespace JumaEngine
{
    struct Vertex2D
    {
        math::vector2 position;
    };

    template<>
    struct VertexInfo<Vertex2D> : std::true_type
    {
        JUMAENGINE_VERTEX_TYPE(Vertex2D)

        static jarray<VertexComponentDescription> getVertexComponents()
        {
            return {{ 0, JSTR("position"), VertexComponentType::Vec2, offsetof(VertexType, position) }};
        }
    };
}
