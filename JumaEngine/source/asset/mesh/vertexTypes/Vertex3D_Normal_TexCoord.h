// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "asset/mesh/VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex3D_Normal_TexCoord
    {
        math::vector3 position;
        math::vector3 normal;
        math::vector2 textureCoords;
    };

    class VertexBufferData_Vertex3D_Normal_TexCoord final : public VertexBufferData<Vertex3D_Normal_TexCoord>
    {
    public:

        virtual jarray<VertexComponentDescription> getVertexComponents() const override
        {
            return {
                { 0, VertexComponentType::Vec3, offsetof(Vertex3D_Normal_TexCoord, position) },
                { 1, VertexComponentType::Vec3, offsetof(Vertex3D_Normal_TexCoord, normal) },
                { 2, VertexComponentType::Vec2, offsetof(Vertex3D_Normal_TexCoord, textureCoords) }
            };
        }

    protected:

        void copyFromImportedVertex(const ImportedVertex& importedVertex, VertexType& outVertex) override
        {
            outVertex.position = importedVertex.position;
            outVertex.normal = importedVertex.normal;
            outVertex.textureCoords = importedVertex.textureCoords;
        }
    };
}
