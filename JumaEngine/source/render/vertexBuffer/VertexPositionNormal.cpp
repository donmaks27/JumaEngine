// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexPositionNormal.h"

namespace JumaEngine
{
    void VertexBufferDataPositionNormal::fillVertexBufferDescription(VertexBufferDescription& description) const
    {
        Super::fillVertexBufferDescription(description);

        description.vertexComponents = {
            { 0, VertexComponentType::Float, 3, offsetof(VertexPositionNormal, position) },
            { 1, VertexComponentType::Float, 3, offsetof(VertexPositionNormal, normal) }
        };
    }

    void VertexBufferDataPositionNormal::copyFromMeshFileImporterData(const MeshFileImporterMeshPartData& data)
    {
        for (const auto& vertexData : data.verticesData)
        {
            vertices.add({ vertexData.position, vertexData.normal });
        }
    }
}
