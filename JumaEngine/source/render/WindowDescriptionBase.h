// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "glm/vec2.hpp"

namespace JumaEngine
{
    class WindowDescriptionBase
    {
    public:
        virtual ~WindowDescriptionBase() = default;

        jstring windowTitle;
        glm::uvec2 windowSize = { 0, 0 };
    };
}
