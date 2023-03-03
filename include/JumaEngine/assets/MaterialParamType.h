// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

#include <JumaRE/material/ShaderUniformInfo.h>

#include "TextureBase.h"
#include "../engine/EngineObjectPtr.h"

namespace JumaEngine
{
    using MaterialParamType = JumaRE::ShaderUniformType;

    template<MaterialParamType Type>
    struct MaterialParamInfo : JumaRE::ShaderUniformInfo<Type>
    {
	    static constexpr JumaRE::ShaderUniformType uniformType = Type;
    };

    template<>
    struct MaterialParamInfo<MaterialParamType::Texture> : std::true_type
    {
        using value_type = EngineObjectPtr<TextureBase>;
        static constexpr JumaRE::ShaderUniformType uniformType = JumaRE::ShaderUniformType::Texture;
    };
}
