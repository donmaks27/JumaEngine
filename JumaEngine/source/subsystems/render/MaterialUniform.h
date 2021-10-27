// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "ShaderUniform.h"
#include <glm/mat4x4.hpp>
#include "utils/jshared_ptr.h"

namespace JumaEngine
{
    class Image;

    struct MaterialUniform
    {
        ShaderUniformType type = ShaderUniformType::None;
        void* data = nullptr;
    };
    template<ShaderUniformType>
    struct MaterialUniformType {};

    struct MaterialUniform_Mat4 final : public MaterialUniform
    {
        MaterialUniform_Mat4()
        {
            type = ShaderUniformType::Mat4;
        }

        using value_type = glm::mat4;
        value_type value = glm::mat4(0);
    };
    template<>
    struct MaterialUniformType<ShaderUniformType::Mat4> { using struct_type = MaterialUniform_Mat4; };

    struct MaterialUniform_Image final : public MaterialUniform
    {
        MaterialUniform_Image()
        {
            type = ShaderUniformType::Image;
        }

        using value_type = jshared_ptr<Image>;
        value_type value = nullptr;
    };
    template<>
    struct MaterialUniformType<ShaderUniformType::Image> { using struct_type = MaterialUniform_Image; };

    namespace MaterialUniformActions
    {
        template<ShaderUniformType Type>
        inline typename MaterialUniformType<Type>::struct_type* create() { return new typename MaterialUniformType<Type>::struct_type(); }
        template<ShaderUniformType Type>
        inline MaterialUniform* create(const typename MaterialUniformType<Type>::struct_type::value_type& value)
        {
            auto* uniform = create<Type>;
            uniform->value = value;
            return uniform;
        }
        inline MaterialUniform* create(const ShaderUniformType type)
        {
            switch (type)
            {
            case ShaderUniformType::Mat4: return create<ShaderUniformType::Mat4>();
            case ShaderUniformType::Image: return create<ShaderUniformType::Image>();
            default: ;
            }
            return nullptr;
        }

        template<ShaderUniformType Type>
        inline bool get(const MaterialUniform* uniform, typename MaterialUniformType<Type>::struct_type::value_type& outValue)
        {
            if ((uniform != nullptr) && (uniform->type == Type))
            {
                outValue = ((const typename MaterialUniformType<Type>::struct_type*)uniform)->value;
                return true;
            }
            return false;
        }

        template<ShaderUniformType Type>
        inline bool set(MaterialUniform* uniform, const typename MaterialUniformType<Type>::struct_type::value_type& value)
        {
            if ((uniform != nullptr) && (uniform->type == Type))
            {
                ((typename MaterialUniformType<Type>::struct_type*)uniform)->value = value;
                return true;
            }
            return false;
        }

        inline void terminate(MaterialUniform* uniform)
        {
            if (uniform != nullptr)
            {
                switch (uniform->type)
                {
                case ShaderUniformType::Mat4: 
                    delete (MaterialUniformType<ShaderUniformType::Mat4>::struct_type*)uniform;
                    break;
                case ShaderUniformType::Image: 
                    delete (MaterialUniformType<ShaderUniformType::Image>::struct_type*)uniform;
                    break;

                default: 
                    delete uniform;
                    break;
                }
            }
        }
    }
}
