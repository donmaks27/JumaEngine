// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    enum class ShaderType
    {
        VERTEX,
        GEOMETRY,
        FRAGMENT
    };

    class ShaderBase
    {
    public:
        ShaderBase();
        virtual ~ShaderBase();
    };
}
