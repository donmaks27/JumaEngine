// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "asset/mesh/VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex3D_Normal_TexCoord
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 textureCoords;
    };

    class VertexBufferData_Vertex3D_Normal_TexCoord final : public VertexBufferData<Vertex3D_Normal_TexCoord>
    {
    public:

        virtual void fillVertexBufferDescription(VertexBufferDescription& description) const override
        {
            Super::fillVertexBufferDescription(description);

            description.vertexComponents = {
                { 0, VertexComponentType::Vec3, offsetof(Vertex3D_Normal_TexCoord, position) },
                { 1, VertexComponentType::Vec3, offsetof(Vertex3D_Normal_TexCoord, normal) },
                { 2, VertexComponentType::Vec2, offsetof(Vertex3D_Normal_TexCoord, textureCoords) }
            };
        }

    protected:

        void copyFromDefaultVertex(const uint32 index, const DefaultVertex& vertex) override
        {
            vertices[index] = { vertex.position, vertex.normal, vertex.textureCoords };
        }
    };
}
