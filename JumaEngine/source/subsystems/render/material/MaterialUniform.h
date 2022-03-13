// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "ShaderUniform.h"
#include "jutils/math/matrix4.h"

namespace JumaEngine
{
    class Texture;

    template<ShaderUniformType Type>
    struct MaterialUniformInfo
    {
        static constexpr bool isValid = false;
    };
    template<>
    struct MaterialUniformInfo<ShaderUniformType::Mat4>
    {
        static constexpr bool isValid = true;
        using value_type = math::matrix4;
    };
    template<>
    struct MaterialUniformInfo<ShaderUniformType::Texture>
    {
        static constexpr bool isValid = true;
        using value_type = Texture*;
    };
}
