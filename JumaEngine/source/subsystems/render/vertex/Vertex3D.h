// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex3D
    {
        math::vector3 position;
    };

    template<>
    struct VertexInfo<Vertex3D> : std::true_type
    {
        JUMAENGINE_VERTEX_TYPE(Vertex3D)

        static jarray<VertexComponentDescription> getVertexComponents()
        {
            return {{ 0, VertexComponentType::Vec3, offsetof(Vertex3D, position) }};
        }

        static void copyFromImportedVertex(VertexType& outVertex, const ImportedVertex& importedVertex)
        {
            outVertex.position = importedVertex.position;
        }
    };
}
