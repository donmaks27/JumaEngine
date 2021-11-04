// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferDescription.h"
#include "DefaultVertexBuffer.h"

namespace JumaEngine
{
    class VertexBufferDataBase
    {
    public:
        VertexBufferDataBase() = default;
        virtual ~VertexBufferDataBase() = default;

        virtual const void* getVertices() const = 0;
        const void* getVertexIndices() const { return !vertexIndices.isEmpty() ? vertexIndices.getData() : nullptr; }

        virtual void fillVertexBufferDescription(VertexBufferDescription& description) const
        {
            description.indicesCount = static_cast<uint32>(vertexIndices.getSize());
        }

        void setVertexIndices(const jarray<uint32>& data) { vertexIndices = data; }
        virtual void copyFromDefaultVertexBuffer(const DefaultVertexBuffer& buffer)
        {
            setVertexIndices(buffer.vertexIndices);
        }

    protected:

        jarray<uint32> vertexIndices;
    };

    template<typename T>
    class VertexBufferData : public VertexBufferDataBase
    {
    public:

        virtual const void* getVertices() const override final { return vertices.getData(); }

        virtual void fillVertexBufferDescription(VertexBufferDescription& description) const override
        {
            VertexBufferDataBase::fillVertexBufferDescription(description);

            description.verticesCount = static_cast<uint32>(vertices.getSize());
            description.vertexSize = sizeof(T);
        }

        void setVertices(const jarray<T>& data) { vertices = data; }
        virtual void copyFromDefaultVertexBuffer(const DefaultVertexBuffer& buffer) override final
        {
            VertexBufferDataBase::copyFromDefaultVertexBuffer(buffer);

            vertices.resize(buffer.vertices.getSize());
            for (uint32 index = 0; index < vertices.getSize(); ++index)
            {
                copyFromDefaultVertex(index, buffer.vertices[index]);
            }
        }

    protected:

        typedef VertexBufferData<T> Super;

        jarray<T> vertices;


        virtual void copyFromDefaultVertex(uint32 index, const DefaultVertex& vertex) = 0;
    };
}
