// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "asset/mesh/VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex2D
    {
        math::vector2 position;
    };

    class VertexBufferData_Vertex2D final : public VertexBufferData<Vertex2D>
    {
    public:

        virtual jarray<VertexComponentDescription> getVertexComponents() const override
        {
            return {{ 0, VertexComponentType::Vec2, offsetof(Vertex2D, position) }};
        }

    protected:

        void copyFromImportedVertex(const ImportedVertex& importedVertex, VertexType& outVertex) override
        {
            outVertex.position = { importedVertex.position.x, importedVertex.position.y };
        }
    };
}
