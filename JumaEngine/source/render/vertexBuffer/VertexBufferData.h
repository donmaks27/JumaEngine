// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "importer/VertexBufferImporterData.h"

namespace JumaEngine
{
    enum class VertexComponentType
    {
        None,
        Float
    };

    class VertexBufferDataBase
    {
    public:
        VertexBufferDataBase() = default;
        virtual ~VertexBufferDataBase() = default;

        std::vector<uint32> vertexIndices;


        virtual void copyFromVertexBufferImporterData(const VertexBufferImporterMeshPartData& data) = 0;

        virtual uint32 getVertexSize() const = 0;
        virtual uint32 getVerticesCount() const { return 0; }

        virtual uint32 getVertexComponentsCount() const = 0;
        virtual int32 getVertexComponentID(const uint32 vertexComponentIndex) const { return -1; }
        virtual VertexComponentType getVertexComponentType(const uint32 vertexComponentIndex) const { return VertexComponentType::None; }
        virtual uint32 getVertexComponentSize(const uint32 vertexComponentIndex) const { return 0; }
        virtual size_t getVertexComponentOffset(const uint32 vertexComponentIndex) const { return 0; }

        virtual const void* getVertices() const { return nullptr; }

        bool shouldUseVertexIndices() const { return !vertexIndices.empty(); }
        uint32 getVertexIndicesCount() const { return static_cast<uint32>(vertexIndices.size()); }
        const void* getVertexIndices() const { return shouldUseVertexIndices() ? vertexIndices.data() : nullptr; }
    };

    template<typename T>
    class VertexBufferData : public VertexBufferDataBase
    {
    public:

        std::vector<T> vertices;


        virtual uint32 getVertexSize() const override final { return sizeof(T); }
        virtual uint32 getVerticesCount() const override final { return static_cast<uint32>(vertices.size()); }

        virtual const void* getVertices() const override final { return getVerticesCount() ? vertices.data() : nullptr; }

    protected:

        typedef VertexBufferData<T> Super;
    };
}
