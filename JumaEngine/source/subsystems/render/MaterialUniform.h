// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "ShaderUniform.h"
#include "utils/type_checks.h"
#include <glm/mat4x4.hpp>

namespace JumaEngine
{
    struct MaterialUniform
    {
        ShaderUniformType type = ShaderUniformType::None;
        void* data = nullptr;
    };

    struct MaterialUniform_Mat4 : public MaterialUniform
    {
        MaterialUniform_Mat4()
        {
            type = ShaderUniformType::Mat4;
        }

        glm::mat4 value = glm::mat4(0);
    };

    template<typename T1>
    constexpr bool is_shader_uniform_type = std::is_same_v<std::remove_cv_t<T1>, glm::mat4>;

    namespace MaterialUniformActions
    {
        template<typename T>
        inline ShaderUniformType getType() { return ShaderUniformType::None; }
        template<>
        inline ShaderUniformType getType<glm::mat4>() { return ShaderUniformType::Mat4; }

        template<typename T>
        inline ShaderUniformType getType(const T&) { return getType<T>(); }

        inline MaterialUniform* create(const glm::mat4& value)
        {
            MaterialUniform_Mat4* uniform = new MaterialUniform_Mat4();
            uniform->value = value;
            return uniform;
        }
        inline MaterialUniform* create(const ShaderUniformType type)
        {
            switch (type)
            {
            case ShaderUniformType::Mat4: return create(glm::mat4(0));
            default: ;
            }
            return nullptr;
        }

        inline bool get(const MaterialUniform* uniform, glm::mat4& outValue)
        {
            if ((uniform != nullptr) && (uniform->type == ShaderUniformType::Mat4))
            {
                outValue = ((const MaterialUniform_Mat4*)uniform)->value;
                return true;
            }
            return false;
        }

        inline bool set(MaterialUniform* uniform, const glm::mat4& value)
        {
            if ((uniform != nullptr) && (uniform->type == ShaderUniformType::Mat4))
            {
                ((MaterialUniform_Mat4*)uniform)->value = value;
                return true;
            }
            return false;
        }

        inline void clear(MaterialUniform* uniform)
        {
            if (uniform != nullptr)
            {
                switch (uniform->type)
                {
                case ShaderUniformType::Mat4: 
                    delete (MaterialUniform_Mat4*)uniform;
                    break;

                default: 
                    delete uniform;
                    break;
                }
            }
        }
    }
}
