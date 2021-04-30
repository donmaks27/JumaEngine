// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexPosition.h"

namespace JumaEngine
{
    void VertexBufferDataPosition::fillVertexBufferDescription(VertexBufferDescription& description) const
    {
        Super::fillVertexBufferDescription(description);

        description.vertexComponents = {
            {0, VertexComponentType::Float, 3, offsetof(VertexPosition, position)}
        };
    }

    void VertexBufferDataPosition::copyFromVertexBufferImporterData(const MeshImporterMeshPartData& data)
    {
        for (const auto& vertexData : data.verticesData)
        {
            vertices.add({ vertexData.position });
        }
    }
}
