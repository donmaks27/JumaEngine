// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "transform/Transform.h"
#include "utils/jarray.h"

namespace JumaEngine
{
    struct VertexBufferImporterVertexData
    {
        glm::vec3 position;
    };

    struct VertexBufferImporterMeshPartData
    {
        jarray<VertexBufferImporterVertexData> verticesData;
    };

    struct VertexBufferImporterMeshData
    {
        std::string name;
        Transform transform;
        jarray<VertexBufferImporterMeshPartData> meshPartsData;
    };

    struct VertexBufferImporterData
    {
        jarray<VertexBufferImporterMeshData> meshesData;
    };
}
