// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "VertexInfo.h"
#include "jutils/math/vector3.h"

namespace JumaEngine
{
    struct Vertex3D_Normal
    {
        math::vector3 position;
        math::vector3 normal;
    };

    template<>
    struct VertexInfo<Vertex3D_Normal> : std::true_type
    {
        JUMAENGINE_VERTEX_TYPE(Vertex3D_Normal)

        static jarray<VertexComponentDescription> getVertexComponents()
        {
            return {
                { 0, JSTR("position"), VertexComponentType::Vec3, offsetof(Vertex3D_Normal, position) },
                { 1, JSTR("normal"), VertexComponentType::Vec3, offsetof(Vertex3D_Normal, normal) }
            };
        }
    };
}
