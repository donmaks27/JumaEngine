// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "ShaderStage.h"
#include "utils/jset.h"

namespace JumaEngine
{
    enum class ShaderUniformType
    {
        None,
        Mat4,
        Texture
    };

    struct ShaderUniform
    {
        jstring name;
        uint32 location = 0;

        ShaderUniformType type = ShaderUniformType::None;
        jset<ShaderStage> stages;
    };
}
