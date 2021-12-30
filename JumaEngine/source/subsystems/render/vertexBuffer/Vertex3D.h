// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex3D
    {
        math::vector3 position;
    };

    class VertexBufferData_Vertex3D final : public VertexBufferData<Vertex3D>
    {
    public:

        virtual jarray<VertexComponentDescription> getVertexComponents() const override
        {
            return {{ 0, VertexComponentType::Vec3, offsetof(Vertex3D, position) }};
        }

    protected:

        void copyFromImportedVertex(const ImportedVertex& importedVertex, VertexType& outVertex) override
        {
            outVertex.position = importedVertex.position;
        }
    };
}
