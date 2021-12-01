// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace JumaEngine
{
    struct DefaultVertex
    {
        glm::vec3 position = glm::vec3(0);
        glm::vec3 normal = glm::vec3(0);
        glm::vec2 textureCoords = glm::vec2(0);
    };

    struct DefaultVertexBuffer
    {
        jarray<DefaultVertex> vertices;
        jarray<uint32> vertexIndices;
    };
}
