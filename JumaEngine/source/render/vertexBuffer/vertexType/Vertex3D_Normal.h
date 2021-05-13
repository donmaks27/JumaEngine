// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "render/vertexBuffer/VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex3D_Normal
    {
        glm::vec3 position;
        glm::vec3 normal;
    };

    class VertexBufferData3D_Normal final : public VertexBufferData<Vertex3D_Normal>
    {
    public:

        virtual void fillVertexBufferDescription(VertexBufferDescription& description) const override
        {
            Super::fillVertexBufferDescription(description);

            description.vertexComponents = {
                { 0, VertexComponentType::Float, 3, offsetof(Vertex3D_Normal, position) },
                { 1, VertexComponentType::Float, 3, offsetof(Vertex3D_Normal, normal) }
            };
        }

        virtual void copyFromMeshFileImporterData(const MeshFileImporterMeshPartData& data) override
        {
            for (const auto& vertexData : data.verticesData)
            {
                vertices.add({ vertexData.position, vertexData.normal });
            }
        }
    };
}
