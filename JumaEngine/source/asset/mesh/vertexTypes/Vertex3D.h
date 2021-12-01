﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "asset/mesh/VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex3D
    {
        math::vector3 position;
    };

    class VertexBufferData_Vertex3D final : public VertexBufferData<Vertex3D>
    {
    public:

        virtual void fillVertexBufferDescription(VertexBufferDescription& description) const override
        {
            Super::fillVertexBufferDescription(description);

            description.vertexComponents = {
                {0, VertexComponentType::Vec3, offsetof(Vertex3D, position)}
            };
        }

    protected:

        void copyFromDefaultVertex(const uint32 index, const DefaultVertex& vertex) override
        {
            vertices[index] = { vertex.position };
        }
    };
}
