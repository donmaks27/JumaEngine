// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex3D_Normal_TexCoord
    {
        math::vector3 position;
        math::vector3 normal;
        math::vector2 textureCoords;
    };

    template<>
    struct VertexInfo<Vertex3D_Normal_TexCoord> : std::true_type
    {
        JUMAENGINE_VERTEX_TYPE(Vertex3D_Normal_TexCoord)

        static jarray<VertexComponentDescription> getVertexComponents()
        {
            return {
                { 0, VertexComponentType::Vec3, offsetof(Vertex3D_Normal_TexCoord, position) },
                { 1, VertexComponentType::Vec3, offsetof(Vertex3D_Normal_TexCoord, normal) },
                { 2, VertexComponentType::Vec2, offsetof(Vertex3D_Normal_TexCoord, textureCoords) }
            };
        }

        static void copyFromImportedVertex(VertexType& outVertex, const ImportedVertex& importedVertex)
        {
            outVertex.position = importedVertex.position;
            outVertex.normal = importedVertex.normal;
            outVertex.textureCoords = importedVertex.textureCoords;
        }
    };
}
