// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "asset/mesh/VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex2D
    {
        glm::vec2 position;
    };

    class VertexBufferData_Vertex2D final : public VertexBufferData<Vertex2D>
    {
    public:

        virtual void fillVertexBufferDescription(VertexBufferDescription& description) const override
        {
            Super::fillVertexBufferDescription(description);

            description.vertexComponents = {
                {0, VertexComponentType::Float, 2, offsetof(Vertex2D, position)}
            };
        }

    protected:

        void copyFromDefaultVertex(const uint32 index, const DefaultVertex& vertex) override
        {
            vertices[index] = { { vertex.position.x, vertex.position.y } };
        }
    };
}
