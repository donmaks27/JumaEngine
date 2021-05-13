// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "render/vertexBuffer/VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex3D
    {
        glm::vec3 position;
    };

    class VertexBufferDataPosition3D final : public VertexBufferData<Vertex3D>
    {
    public:

        virtual void fillVertexBufferDescription(VertexBufferDescription& description) const override
        {
            Super::fillVertexBufferDescription(description);

            description.vertexComponents = {
                {0, VertexComponentType::Float, 3, offsetof(Vertex3D, position)}
            };
        }

        virtual void copyFromMeshFileImporterData(const MeshFileImporterMeshPartData& data) override
        {
            for (const auto& vertexData : data.verticesData)
            {
                vertices.add({ vertexData.position });
            }
        }
    };
}
