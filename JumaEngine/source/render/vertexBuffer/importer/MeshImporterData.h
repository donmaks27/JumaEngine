// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "transform/Transform.h"
#include "utils/jarray.h"

namespace JumaEngine
{
    struct MeshImporterVertexData
    {
        glm::vec3 position;
    };

    struct MeshImporterMeshPartData
    {
        jarray<MeshImporterVertexData> verticesData;
    };

    struct MeshImporterMeshData
    {
        jstring name;
        Transform transform;
        jarray<MeshImporterMeshPartData> meshPartsData;
    };

    struct MeshImporterData
    {
        jarray<MeshImporterMeshData> meshesData;
    };
}
