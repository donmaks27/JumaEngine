// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "ShaderUniform.h"
#include "jutils/math/matrix4.h"

namespace JumaEngine
{
    class Texture;
    class RenderTarget;

    template<ShaderUniformType Type>
    struct MaterialParamInfo
    {
        static constexpr bool isValid = false;
    };

    template<>
    struct MaterialParamInfo<ShaderUniformType::Mat4>
    {
        static constexpr bool isValid = true;
        using value_type = math::matrix4;
    };
    template<>
    struct MaterialParamInfo<ShaderUniformType::Texture>
    {
        static constexpr bool isValid = true;
        using value_type = Texture*;
    };
    template<>
    struct MaterialParamInfo<ShaderUniformType::RenderTarget>
    {
        static constexpr bool isValid = true;
        using value_type = RenderTarget*;
    };
}
