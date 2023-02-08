// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

#include <JumaRE/vertex/VertexBuffer.h>

namespace JumaEngine
{
    class Material;

    struct VertexBuffer
    {
        JumaRE::VertexBuffer* vertexBuffer = nullptr;
        Material* material = nullptr;
    };
}
