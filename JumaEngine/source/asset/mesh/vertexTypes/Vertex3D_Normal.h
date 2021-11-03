// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "asset/mesh/VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex3D_Normal
    {
        glm::vec3 position;
        glm::vec3 normal;
    };

    class VertexBufferData_Vertex3D_Normal final : public VertexBufferData<Vertex3D_Normal>
    {
    public:

        virtual void fillVertexBufferDescription(VertexBufferDescription& description) const override
        {
            Super::fillVertexBufferDescription(description);

            description.vertexComponents = {
                { 0, VertexComponentType::Vec3, offsetof(Vertex3D_Normal, position) },
                { 1, VertexComponentType::Vec3, offsetof(Vertex3D_Normal, normal) }
            };
        }

    protected:

        void copyFromDefaultVertex(const uint32 index, const DefaultVertex& vertex) override
        {
            vertices[index] = { vertex.position, vertex.normal };
        }
    };
}
