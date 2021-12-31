// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex2D
    {
        math::vector2 position;
    };

    template<>
    struct VertexTypeDescription<Vertex2D> : std::true_type
    {
        JUMAENGINE_VERTEX_TYPE(Vertex2D)

        static jarray<VertexComponentDescription> getVertexComponents()
        {
            return {{ 0, VertexComponentType::Vec2, offsetof(VertexType, position) }};
        }

        static void copyFromImportedVertex(VertexType& outVertex, const ImportedVertex& importedVertex)
        {
            outVertex.position = { importedVertex.position.x, importedVertex.position.y };
        }
    };
}
