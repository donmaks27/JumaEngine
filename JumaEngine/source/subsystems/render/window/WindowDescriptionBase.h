// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include <glm/vec2.hpp>

namespace JumaEngine
{
    struct WindowDescriptionBase
    {
    protected:
        WindowDescriptionBase() = default;
    public:

        jstring title;
        glm::uvec2 size = { 0, 0 };
    };
}
