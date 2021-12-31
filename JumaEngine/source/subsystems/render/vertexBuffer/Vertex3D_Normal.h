// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex3D_Normal
    {
        math::vector3 position;
        math::vector3 normal;
    };

    template<>
    struct VertexTypeDescription<Vertex3D_Normal> : std::true_type
    {
        JUMAENGINE_VERTEX_TYPE(Vertex3D_Normal)

        static jarray<VertexComponentDescription> getVertexComponents()
        {
            return {
                { 0, VertexComponentType::Vec3, offsetof(Vertex3D_Normal, position) },
                { 1, VertexComponentType::Vec3, offsetof(Vertex3D_Normal, normal) }
            };
        }

        static void copyFromImportedVertex(VertexType& outVertex, const ImportedVertex& importedVertex)
        {
            outVertex.position = importedVertex.position;
            outVertex.normal = importedVertex.normal;
        }
    };
}
