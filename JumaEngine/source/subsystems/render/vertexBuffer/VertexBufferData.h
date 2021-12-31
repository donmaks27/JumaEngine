// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBase.h"
#include "ImportedVertex.h"
#include "VertexComponents.h"

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

    template<typename T, TEMPLATE_ENABLE(is_vertex_type<T>)>
    class VertexBufferData final : public VertexBufferDataBase
    {
    public:
        VertexBufferData() = default;
        virtual ~VertexBufferData() override = default;

        using VertexType = T;
        using VertexDescription = VertexTypeDescription<VertexType>;

        virtual const void* getVertices() const override { return vertices.getData(); }
        virtual uint32 getVertexCount() const override { return static_cast<uint32>(vertices.getSize()); }
        
        virtual uint32 getVertexSize() const override { return sizeof(VertexType); }
        virtual jarray<VertexComponentDescription> getVertexComponents() const override { return VertexDescription::getVertexComponents(); }

        void setVertices(std::initializer_list<VertexType> list) { vertices = list; }
        void setVertices(const jarray<VertexType>& data) { vertices = data; }

        virtual void copyFromImportedVertexBuffer(const ImportedVertexBuffer& buffer) override
        {
            VertexBufferDataBase::copyFromImportedVertexBuffer(buffer);

            const int32 size = buffer.vertices.getSize();
            vertices.resize(size);
            for (int32 index = 0; index < size; index++)
            {
                VertexDescription::copyFromImportedVertex(vertices[index], buffer.vertices[index]);
            }
        }

    private:

        jarray<VertexType> vertices;
    };
}
