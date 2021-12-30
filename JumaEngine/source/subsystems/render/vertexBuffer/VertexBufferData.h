// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexComponents.h"
#include "ImportedVertexBuffer.h"

namespace JumaEngine
{
    class VertexBufferDataBase
    {
    public:
        VertexBufferDataBase() = default;
        virtual ~VertexBufferDataBase() = default;

        virtual const void* getVertices() const = 0;
        virtual uint32 getVertexCount() const = 0;

        const void* getIndices() const { return !vertexIndices.isEmpty() ? vertexIndices.getData() : nullptr; }
        uint32 getIndexCount() const { return static_cast<uint32>(vertexIndices.getSize()); }

        virtual uint32 getVertexSize() const = 0;
        virtual jarray<VertexComponentDescription> getVertexComponents() const = 0;

        void setVertexIndices(std::initializer_list<uint32> list) { vertexIndices = list; }
        void setVertexIndices(const jarray<uint32>& data) { vertexIndices = data; }

        virtual void copyFromImportedVertexBuffer(const ImportedVertexBuffer& buffer)
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
        virtual uint32 getVertexCount() const override final { return static_cast<uint32>(vertices.getSize()); }
        
        virtual uint32 getVertexSize() const override final { return sizeof(T); }

        void setVertices(std::initializer_list<T> list) { vertices = list; }
        void setVertices(const jarray<T>& data) { vertices = data; }

        virtual void copyFromImportedVertexBuffer(const ImportedVertexBuffer& buffer) override final
        {
            VertexBufferDataBase::copyFromImportedVertexBuffer(buffer);

            vertices.resize(buffer.vertices.getSize());
            for (int32 index = 0; index < vertices.getSize(); ++index)
            {
                copyFromImportedVertex(buffer.vertices[index], vertices[index]);
            }
        }

    protected:

        using Super = VertexBufferData<T>;
        using VertexType = T;

        jarray<T> vertices;


        virtual void copyFromImportedVertex(const ImportedVertex& importedVertex, VertexType& outVertex) = 0;
    };
}
