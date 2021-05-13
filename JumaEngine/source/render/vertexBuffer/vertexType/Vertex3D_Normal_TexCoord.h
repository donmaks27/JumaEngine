// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "render/vertexBuffer/VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex3D_Normal_TexCoord
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 textureCoords;
    };

    class VertexBufferData3D_Normal_TexCoord final : public VertexBufferData<Vertex3D_Normal_TexCoord>
    {
    public:

        virtual void fillVertexBufferDescription(VertexBufferDescription& description) const override
        {
            Super::fillVertexBufferDescription(description);

            description.vertexComponents = {
                { 0, VertexComponentType::Float, 3, offsetof(Vertex3D_Normal_TexCoord, position) },
                { 1, VertexComponentType::Float, 3, offsetof(Vertex3D_Normal_TexCoord, normal) },
                { 2, VertexComponentType::Float, 2, offsetof(Vertex3D_Normal_TexCoord, textureCoords) }
            };
        }

        virtual void copyFromMeshFileImporterData(const MeshFileImporterMeshPartData& data) override
        {
            for (const auto& vertexData : data.verticesData)
            {
                vertices.add({ vertexData.position, vertexData.normal, vertexData.textureCoords });
            }
        }
    };
}
