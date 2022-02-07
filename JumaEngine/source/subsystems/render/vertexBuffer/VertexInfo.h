// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexComponents.h"
#include "ImportedVertex.h"
#include "jutils/jstringID.h"

namespace JumaEngine
{
    template<typename T>
    struct VertexInfo : std::false_type
    {
        // static jarray<VertexComponentDescription> getVertexComponents();

        // static void copyFromImportedVertex(T& outVertex, const ImportedVertex& importedVertex);
    };

    template<typename T>
    constexpr bool is_vertex_type = VertexInfo<T>::value;
}

#define JUMAENGINE_VERTEX_TYPE(VertexName)                      \
    using VertexType = VertexName;                              \
    static const jstringID& getVertexName()                     \
    {                                                           \
        static jstringID vertexName = #VertexName;              \
        return vertexName;                                      \
    }                                                           \
    static uint32 getVertexSize() { return sizeof(VertexName); }
