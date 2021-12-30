// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "asset/mesh/VertexBufferData.h"

namespace JumaEngine
{
    struct Vertex3D_Normal
    {
        math::vector3 position;
        math::vector3 normal;
    };

    class VertexBufferData_Vertex3D_Normal final : public VertexBufferData<Vertex3D_Normal>
    {
    public:

        virtual jarray<VertexComponentDescription> getVertexComponents() const override
        {
            return {
                { 0, VertexComponentType::Vec3, offsetof(Vertex3D_Normal, position) },
                { 1, VertexComponentType::Vec3, offsetof(Vertex3D_Normal, normal) }
            };
        }

    protected:

        void copyFromImportedVertex(const ImportedVertex& importedVertex, VertexType& outVertex) override
        {
            outVertex.position = importedVertex.position;
            outVertex.normal = importedVertex.normal;
        }
    };
}
