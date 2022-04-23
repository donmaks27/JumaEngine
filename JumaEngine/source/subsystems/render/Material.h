// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderAPIObject.h"
#include "engine/EngineContextObject.h"

#include "jutils/jdelegate_multicast.h"
#include "material/MaterialParamsStorage.h"

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

        template<ShaderUniformType Type, TEMPLATE_ENABLE(MaterialParamInfo<Type>::isValid)>
        bool getParamValue(const jstringID& paramName, typename MaterialParamInfo<Type>::value_type& outValue) const
        {
            if (!isUniformTypeCorrect(paramName, Type))
            {
                return false;
            }
            return this->getParamValueInternal<Type>(paramName, outValue);
        }
        template<ShaderUniformType Type, TEMPLATE_ENABLE(MaterialParamInfo<Type>::isValid)>
        bool setParamValue(const jstringID& paramName, const typename MaterialParamInfo<Type>::value_type& value)
        {
            return isUniformTypeCorrect(paramName, Type) && m_MaterialParams.setValue<Type>(paramName, value);
        }
        void resetParamValue(const jstringID& paramName);

    protected:

        virtual Material_RenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearInternal() override { clearData(); }

    private:

        Shader* m_BaseShader = nullptr;
        Material* m_BaseMaterial = nullptr;

        MaterialParamsStorage m_MaterialParams;


        void clearData();

        void onBaseShaderClear(Shader* shader) { clear(); }
        void onBaseMaterialClear(Material* material) { clear(); }
        
        bool isUniformTypeCorrect(const jstringID& paramName, ShaderUniformType type) const;
        template<ShaderUniformType Type>
        bool getParamValueInternal(const jstringID& paramName, typename MaterialParamInfo<Type>::value_type& outValue) const
        {
            if (m_MaterialParams.getValue<Type>(paramName, outValue))
            {
                return true;
            }
            return isMaterialInstance() && getBaseMaterial()->getParamValueInternal<Type>(paramName, outValue);
        }
    };
}
