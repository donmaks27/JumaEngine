// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "VertexComponents.h"

namespace JumaEngine
{
    template<typename T>
    struct VertexInfo : std::false_type
    {
        static const jstringID& getVertexName()
        {
            static jstringID vertexName = jstringID_NONE;
            return vertexName;
        }
        static uint32 getVertexSize() { return 0; }

        static jarray<VertexComponentDescription> getVertexComponents() { return {}; }
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
