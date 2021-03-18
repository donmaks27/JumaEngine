// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexPosition.h"

namespace JumaEngine
{
    int32 VertexBufferDataPosition::getVertexComponentID(const uint32 vertexComponentIndex) const
    {
        return vertexComponentIndex == 0 ? 0 : Super::getVertexComponentID(vertexComponentIndex);
    }
    VertexComponentType VertexBufferDataPosition::getVertexComponentType(const uint32 vertexComponentIndex) const
    {
        return vertexComponentIndex == 0 ? VertexComponentType::Float : Super::getVertexComponentType(vertexComponentIndex);
    }
    uint32 VertexBufferDataPosition::getVertexComponentSize(const uint32 vertexComponentIndex) const
    {
        return vertexComponentIndex == 0 ? 2 : Super::getVertexComponentSize(vertexComponentIndex);
    }
    size_t VertexBufferDataPosition::getVertexComponentOffset(const uint32 vertexComponentIndex) const
    {
        return vertexComponentIndex == 0 ? offsetof(VertexPosition, position) : Super::getVertexComponentOffset(vertexComponentIndex);
    }
}
