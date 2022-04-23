// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderAPIObject.h"
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

    class Material_RenderAPIObject : public RenderAPIObject<Material>
    {
        friend Material;

    public:
        Material_RenderAPIObject() = default;
        virtual ~Material_RenderAPIObject() override = default;

        virtual bool render(VertexBuffer* vertexBuffer, const RenderOptions* renderOptions) = 0;
    };

    CREATE_JUTILS_MULTICAST_DELEGATE_OneParam(OnMaterialEvent, Material*, material);
    CREATE_JUTILS_MULTICAST_DELEGATE_TwoParams(OnMaterialParamEvent, Material*, material, const jstringID&, paramName);

    class Material final : public EngineContextObject, public RenderAPIWrapper<Material_RenderAPIObject>
    {
        JUMAENGINE_CLASS(Material, EngineContextObject)

    public:
        Material() = default;
        virtual ~Material() override;

        OnMaterialEvent onClear;


        bool init(Shader* shader);
        bool init(Material* baseMaterial);

        Shader* getShader() const { return m_BaseShader; }
        Material* getBaseMaterial() const { return m_BaseMaterial; }
        bool isMaterialInstance() const { return isValid() && (getBaseMaterial() != nullptr); }

        bool render(VertexBuffer* vertexBuffer, const RenderOptions* renderOptions);

        ShaderUniformType getParamType(const jstringID& paramName) const;
        bool isOverrideParam(const jstringID& paramName) const;

        template<ShaderUniformType Type, TEMPLATE_ENABLE(MaterialUniformInfo<Type>::isValid)>
        bool getParamValue(const jstringID& paramName, typename MaterialUniformInfo<Type>::value_type& outValue) const
        {
            if (!isUniformTypeCorrect(paramName, Type))
            {
                return false;
            }
            if (this->getParamValueInternal<Type>(paramName, outValue))
            {
                return true;
            }
            return isMaterialInstance() && m_BaseMaterial->getParamValueInternal<Type>(paramName, outValue);
        }
        template<ShaderUniformType Type, TEMPLATE_ENABLE(MaterialUniformInfo<Type>::isValid)>
        bool setParamValue(const jstringID& paramName, const typename MaterialUniformInfo<Type>::value_type& value)
        {
            return !isUniformTypeCorrect(paramName, Type) || !this->setParamValueInternal<Type>(paramName, value);
        }
        void resetParamValue(const jstringID& paramName);

    protected:

        virtual Material_RenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearInternal() override { clearData(); }

    private:

        using mat4_value_type = MaterialUniformInfo<ShaderUniformType::Mat4>::value_type;
        using texture_value_type = MaterialUniformInfo<ShaderUniformType::Texture>::value_type;
        using render_target_value_type = MaterialUniformInfo<ShaderUniformType::RenderTarget>::value_type;

        Shader* m_BaseShader = nullptr;
        Material* m_BaseMaterial = nullptr;

        jmap<jstringID, mat4_value_type> m_UniformValues_Mat4;
        jmap<jstringID, texture_value_type> m_UniformValues_Texture;
        jmap<jstringID, render_target_value_type> m_UniformValues_RenderTarget;


        void clearData();

        void createUniformValues(Shader* shader);
        void clearUniformValues();

        void onBaseShaderClear(Shader* shader) { clear(); }
        void onBaseMaterialClear(Material* material) { clear(); }
        
        bool isUniformTypeCorrect(const jstringID& paramName, ShaderUniformType type) const;

        template<ShaderUniformType Type>
        bool getParamValueInternal(const jstringID& paramName, typename MaterialUniformInfo<Type>::value_type& outValue) const { return false; }
        template<>
        bool getParamValueInternal<ShaderUniformType::Mat4>(const jstringID& paramName, mat4_value_type& outValue) const { return getParamValueInternal<ShaderUniformType::Mat4>(m_UniformValues_Mat4, paramName, outValue); }
        template<>
        bool getParamValueInternal<ShaderUniformType::Texture>(const jstringID& paramName, texture_value_type& outValue) const { return getParamValueInternal<ShaderUniformType::Texture>(m_UniformValues_Texture, paramName, outValue); }
        template<>
        bool getParamValueInternal<ShaderUniformType::RenderTarget>(const jstringID& paramName, render_target_value_type& outValue) const { return getParamValueInternal<ShaderUniformType::RenderTarget>(m_UniformValues_RenderTarget, paramName, outValue); }
        template<ShaderUniformType Type>
        static bool getParamValueInternal(const jmap<jstringID, typename MaterialUniformInfo<Type>::value_type>& valuesMap, 
            const jstringID& paramName, typename MaterialUniformInfo<Type>::value_type& outValue)
        {
            const auto* value = valuesMap.find(paramName);
            if (value != nullptr)
            {
                outValue = *value;
                return true;
            }
            return false;
        }

        template<ShaderUniformType Type>
        bool setParamValueInternal(const jstringID& paramName, const typename MaterialUniformInfo<Type>::value_type& value) { return false; }
        template<>
        bool setParamValueInternal<ShaderUniformType::Mat4>(const jstringID& paramName, const mat4_value_type& value)
        {
            m_UniformValues_Mat4[paramName] = value;
            return true;
        }
        template<>
        bool setParamValueInternal<ShaderUniformType::Texture>(const jstringID& paramName, const texture_value_type& value) { return setParamValueInternal_Texture(paramName, value); }
        template<>
        bool setParamValueInternal<ShaderUniformType::RenderTarget>(const jstringID& paramName, const render_target_value_type& value) { return setParamValueInternal_RenderTarget(paramName, value); }

        bool setParamValueInternal_Texture(const jstringID& paramName, const texture_value_type& value);
        void onTextureCleared(Texture* texture);

        bool setParamValueInternal_RenderTarget(const jstringID& paramName, const render_target_value_type& value);
        void onRenderTargetCleared(RenderTarget* texture);
    };
}
