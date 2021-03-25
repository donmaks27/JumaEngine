// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "transform/Transform.h"
#include <vector>

namespace JumaEngine
{
    struct VertexBufferImporterVertexData
    {
        glm::vec3 position;
    };

    struct VertexBufferImporterMeshPartData
    {
        std::vector<VertexBufferImporterVertexData> verticesData;
    };

    struct VertexBufferImporterMeshData
    {
        std::string name;
        Transform transform;
        std::vector<VertexBufferImporterMeshPartData> meshPartsData;
    };

    struct VertexBufferImporterData
    {
        std::vector<VertexBufferImporterMeshData> meshesData;
    };
}
