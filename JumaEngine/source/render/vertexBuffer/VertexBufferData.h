// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferDescription.h"
#include "asset/mesh/MeshFileImporterData.h"

namespace JumaEngine
{
    class VertexBufferDataBase
    {
    public:
        VertexBufferDataBase() = default;
        virtual ~VertexBufferDataBase() = default;

        virtual const void* getVertices() const { return nullptr; }
        const void* getVertexIndices() const { return !vertexIndices.empty() ? vertexIndices.data() : nullptr; }

        virtual void fillVertexBufferDescription(VertexBufferDescription& description) const
        {
            description.indicesCount = static_cast<uint32>(vertexIndices.size());
        }

        virtual void copyFromMeshFileImporterData(const MeshFileImporterMeshPartData& data) = 0;

    protected:

        jarray<uint32> vertexIndices;
    };

    template<typename T>
    class VertexBufferData : public VertexBufferDataBase
    {
    public:

        virtual const void* getVertices() const override final { return vertices.data(); }

        virtual void fillVertexBufferDescription(VertexBufferDescription& description) const override
        {
            VertexBufferDataBase::fillVertexBufferDescription(description);

            description.verticesCount = static_cast<uint32>(vertices.size());
            description.vertexSize = sizeof(T);
        }

    protected:

        typedef VertexBufferData<T> Super;

        jarray<T> vertices;
    };
}
