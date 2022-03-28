// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderObject.h"
#include "engine/EngineContextObject.h"

#include "jutils/jdelegate_multicast.h"
#include "jutils/jmap.h"
#include "jutils/jstringID.h"
#include "material/MaterialUniform.h"

namespace JumaEngine
{
    struct RenderOptions;
    class Shader;
    class Material;
    class VertexBuffer;

    class MaterialObject : public RenderObject<Material>
    {
        friend Material;

    public:
        MaterialObject() = default;
        virtual ~MaterialObject() override = default;

        virtual bool render(VertexBuffer* vertexBuffer, const RenderOptions* renderOptions) = 0;

    protected:

        virtual void onMaterialParamChanged(const jstringID& paramName) {}
    };

    CREATE_JUTILS_MULTICAST_DELEGATE_OneParam(OnMaterialEvent, Material*, material);
    CREATE_JUTILS_MULTICAST_DELEGATE_TwoParams(OnMaterialParamEvent, Material*, material, const jstringID&, paramName);

    class Material final : public EngineContextObject
    {
        JUMAENGINE_CLASS(Material, EngineContextObject)

    public:
        Material() = default;
        virtual ~Material() override;

        OnMaterialEvent onClear;
        OnMaterialParamEvent onParamChanged;


        bool init(Shader* shader);
        bool init(Material* baseMaterial);
        bool isValid() const { return m_Initialized; }
        void clear();

        Shader* getShader() const { return m_BaseShader; }
        Material* getBaseMaterial() const { return m_BaseMaterial; }
        bool isMaterialInstance() const { return isValid() && (getBaseMaterial() != nullptr); }

        bool createRenderObject();
        MaterialObject* getRenderObject() const { return m_RenderObject; }
        void clearRenderObject();

        bool render(VertexBuffer* vertexBuffer, const RenderOptions* renderOptions);

        ShaderUniformType getParamType(const jstringID& paramName) const;
        bool isOverrideParam(const jstringID& paramName) const;
        template<ShaderUniformType Type, TEMPLATE_ENABLE(MaterialUniformInfo<Type>::isValid)>
        bool getParamValue(const jstringID& paramName, typename MaterialUniformInfo<Type>::value_type& outValue) const;
        template<ShaderUniformType Type, TEMPLATE_ENABLE(MaterialUniformInfo<Type>::isValid)>
        bool setParamValue(const jstringID& paramName, const typename MaterialUniformInfo<Type>::value_type& value);
        void resetParamValue(const jstringID& paramName);

    private:

        using mat4_value_type = MaterialUniformInfo<ShaderUniformType::Mat4>::value_type;
        using texture_value_type = MaterialUniformInfo<ShaderUniformType::Texture>::value_type;

        bool m_Initialized = false;

        Shader* m_BaseShader = nullptr;
        Material* m_BaseMaterial = nullptr;

        jmap<jstringID, mat4_value_type> m_UniformValues_Mat4;
        jmap<jstringID, texture_value_type> m_UniformValues_Texture;

        MaterialObject* m_RenderObject = nullptr;


        void createUniformValues(Shader* shader);
        void clearUniformValues();

        void onBaseShaderClear(Shader* shader) { clear(); }
        void onBaseMaterialClear(Material* material) { clear(); }
        
        void onBaseMaterialParamChanged(Material* material, const jstringID& paramName);
        void notifyMaterialParamChanged(const jstringID& paramName);

        bool isUniformTypeCorrect(const jstringID& paramName, ShaderUniformType type) const;

        template<ShaderUniformType Type>
        bool getParamValueInternal(const jstringID& paramName, typename MaterialUniformInfo<Type>::value_type& outValue) const { return false; }
        template<>
        inline bool getParamValueInternal<ShaderUniformType::Mat4>(const jstringID& paramName, mat4_value_type& outValue) const;
        template<>
        inline bool getParamValueInternal<ShaderUniformType::Texture>(const jstringID& paramName, texture_value_type& outValue) const;

        template<ShaderUniformType Type>
        bool setParamValueInternal(const jstringID& paramName, const typename MaterialUniformInfo<Type>::value_type& value) { return false; }
        template<>
        inline bool setParamValueInternal<ShaderUniformType::Mat4>(const jstringID& paramName, const mat4_value_type& value);
        template<>
        inline bool setParamValueInternal<ShaderUniformType::Texture>(const jstringID& paramName, const texture_value_type& value);
    };

    template <ShaderUniformType Type, TEMPLATE_ENABLE_IMPL(MaterialUniformInfo<Type>::isValid)>
    bool Material::getParamValue(const jstringID& paramName, typename MaterialUniformInfo<Type>::value_type& outValue) const
    {
        if (!isUniformTypeCorrect(paramName, Type))
        {
            return false;
        }
        if (getParamValueInternal<Type>(paramName, outValue))
        {
            return true;
        }
        return isMaterialInstance() && m_BaseMaterial->getParamValueInternal<Type>(paramName, outValue);
    }
    template<>
    bool Material::getParamValueInternal<ShaderUniformType::Mat4>(const jstringID& paramName, mat4_value_type& outValue) const
    {
        const MaterialUniformInfo<ShaderUniformType::Mat4>::value_type* value = m_UniformValues_Mat4.find(paramName);
        if (value != nullptr)
        {
            outValue = *value;
            return true;
        }
        return false;
    }
    template<>
    bool Material::getParamValueInternal<ShaderUniformType::Texture>(const jstringID& paramName, texture_value_type& outValue) const
    {
        const MaterialUniformInfo<ShaderUniformType::Texture>::value_type* value = m_UniformValues_Texture.find(paramName);
        if (value != nullptr)
        {
            outValue = *value;
            return true;
        }
        return false;
    }

    template <ShaderUniformType Type, TEMPLATE_ENABLE_IMPL(MaterialUniformInfo<Type>::isValid)>
    bool Material::setParamValue(const jstringID& paramName, const typename MaterialUniformInfo<Type>::value_type& value)
    {
        if (!isUniformTypeCorrect(paramName, Type) || !setParamValueInternal<Type>(paramName, value))
        {
            return false;
        }
        notifyMaterialParamChanged(paramName);
        return true;
    }
    template<>
    bool Material::setParamValueInternal<ShaderUniformType::Mat4>(const jstringID& paramName, const mat4_value_type& value)
    {
        m_UniformValues_Mat4[paramName] = value;
        return true;
    }
    template<>
    bool Material::setParamValueInternal<ShaderUniformType::Texture>(const jstringID& paramName, const texture_value_type& value)
    {
        m_UniformValues_Texture[paramName] = value;
        return true;
    }
}
