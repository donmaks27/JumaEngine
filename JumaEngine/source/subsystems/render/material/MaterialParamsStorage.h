// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "MaterialParam.h"
#include "jutils/jmap.h"
#include "jutils/jstringID.h"

namespace JumaEngine
{
    class MaterialParamsStorage final
    {
    public:
        MaterialParamsStorage() = default;
        ~MaterialParamsStorage();

        template<ShaderUniformType Type>
        bool setValue(const jstringID& name, const typename MaterialParamInfo<Type>::value_type& value)
        {
            return (name != jstringID_NONE) && this->setValueInternal<Type>(name, value);
        }
        bool setDefaultValue(ShaderUniformType type, const jstringID& name);
        void removeValue(ShaderUniformType type, const jstringID& name);

        template<ShaderUniformType Type>
        bool getValue(const jstringID& name, typename MaterialParamInfo<Type>::value_type& outValue) const
        {
            const typename MaterialParamInfo<Type>::value_type* valuePtr = this->findValue<Type>(name);
            if (valuePtr == nullptr)
            {
                return false;
            }
            outValue = *valuePtr;
            return true;
        }
        bool contains(ShaderUniformType type, const jstringID& name) const;
        template<ShaderUniformType Type>
        bool contains(const jstringID& name) const { return contains(Type, name); }

        void clear();

    private:

        template<ShaderUniformType Type>
        using material_params_map = jmap<jstringID, typename MaterialParamInfo<Type>::value_type>;

        material_params_map<ShaderUniformType::Mat4> m_UniformValues_Mat4;
        material_params_map<ShaderUniformType::Texture> m_UniformValues_Texture;
        material_params_map<ShaderUniformType::RenderTarget> m_UniformValues_RenderTarget;


        template<ShaderUniformType Type>
        bool setValueInternal(const jstringID& name, const typename MaterialParamInfo<Type>::value_type& value) { return false; }
        template<>
        bool setValueInternal<ShaderUniformType::Mat4>(const jstringID& name, const MaterialParamInfo<ShaderUniformType::Mat4>::value_type& value)
        {
            m_UniformValues_Mat4[name] = value;
            return true;
        }
        template<>
        bool setValueInternal<ShaderUniformType::Texture>(const jstringID& name, const MaterialParamInfo<ShaderUniformType::Texture>::value_type& value)
        {
            setValueInternal_Texture(name, value);
            return true;
        }
        template<>
        bool setValueInternal<ShaderUniformType::RenderTarget>(const jstringID& name, const MaterialParamInfo<ShaderUniformType::RenderTarget>::value_type& value)
        {
            setValueInternal_RenderTarget(name, value);
            return true;
        }

        void setValueInternal_Texture(const jstringID& name, Texture* value);
        void onTextureCleared(Texture* texture);

        void setValueInternal_RenderTarget(const jstringID& name, RenderTarget* value);
        void onRenderTargetCleared(RenderTarget* renderTarget);

        template<ShaderUniformType Type>
        const typename MaterialParamInfo<Type>::value_type* findValue(const jstringID& name) const { return nullptr; }
        template<>
        const MaterialParamInfo<ShaderUniformType::Mat4>::value_type* findValue<ShaderUniformType::Mat4>(const jstringID& name) const { return m_UniformValues_Mat4.find(name); }
        template<>
        const MaterialParamInfo<ShaderUniformType::Texture>::value_type* findValue<ShaderUniformType::Texture>(const jstringID& name) const { return m_UniformValues_Texture.find(name); }
        template<>
        const MaterialParamInfo<ShaderUniformType::RenderTarget>::value_type* findValue<ShaderUniformType::RenderTarget>(const jstringID& name) const { return m_UniformValues_RenderTarget.find(name); }
    };
}
