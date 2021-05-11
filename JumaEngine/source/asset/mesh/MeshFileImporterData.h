// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "transform/Transform.h"
#include "utils/jarray.h"

namespace JumaEngine
{
    struct MeshFileImporterVertexData
    {
        glm::vec3 position;
        glm::vec3 normal;
    };

    struct MeshFileImporterMeshPartData
    {
        jarray<MeshFileImporterVertexData> verticesData;
    };

    struct MeshFileImporterMeshData
    {
        jstring name;
        Transform transform;
        jarray<MeshFileImporterMeshPartData> meshPartsData;
    };

    struct MeshFileImporterData
    {
        jarray<MeshFileImporterMeshData> meshesData;
    };
}
