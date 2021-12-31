// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    template<typename T>
    struct VertexTypeDescription : std::false_type {};

    template<typename T>
    constexpr bool is_vertex_type = VertexTypeDescription<T>::value;
}

#define JUMAENGINE_VERTEX_TYPE(VertexName) \
    using VertexType = VertexName; \
    static jstring getVertexName() { return #VertexName; }
