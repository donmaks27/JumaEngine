// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../../EngineContextObject.h"

#include <JumaRE/material/Material.h>
#include <jutils/jdelegate_multicast.h>

namespace JumaEngine
{
    class Material;
    class Shader;
    class ShadersSubsystem;

    JUTILS_CREATE_MULTICAST_DELEGATE1(OnMaterialEvent, Material*, material);
    JUTILS_CREATE_MULTICAST_DELEGATE2(OnMaterialParamEvent, Material*, material, const jstringID&, paramName);

    class Material final : public EngineContextObject
    {
        JUMAENGINE_CLASS(Material, EngineContextObject)

        friend ShadersSubsystem;

    public:
        Material() = default;
        virtual ~Material() override = default;

        OnMaterialEvent onClear;
        OnMaterialParamEvent onParamChanged;


        Material* getBaseMaterial() const { return m_BaseMaterial; }
        Shader* getBaseShader() const { return m_BaseShader != nullptr ? m_BaseShader : (m_BaseMaterial != nullptr ? m_BaseMaterial->getBaseShader() : nullptr); }

        JumaRE::Material* getMaterial() const { return m_Material; }
        JumaRE::Shader* getShader() const { return m_Material != nullptr ? m_Material->getShader() : nullptr; }

        template<JumaRE::ShaderUniformType Type>
        bool setParamValue(const jstringID& name, const typename JumaRE::ShaderUniformInfo<Type>::value_type& value)
        {
            if (isEngineInternalMaterialParam(name) || !m_Material->setParamValue<Type>(name, value))
            {
                return false;
            }
            onParamChanged.call(this, name);
            return true;
        }
        bool resetParamValue(const jstringID& name)
        {
            if (isEngineInternalMaterialParam(name) || !m_Material->resetParamValue(name))
            {
                return false;
            }
            onParamChanged.call(this, name);
            return true;
        }
        template<JumaRE::ShaderUniformType Type>
        bool getParamValue(const jstringID& name, typename JumaRE::ShaderUniformInfo<Type>::value_type& outValue) const { return m_Material->getParamValue(name, outValue); }

    private:

        Shader* m_BaseShader = nullptr;
        Material* m_BaseMaterial = nullptr;

        JumaRE::Material* m_Material = nullptr;


        bool createMaterial(Shader* shader);
        bool createMaterial(Material* baseMaterial);
        void clearMaterial();

        void onBaseMaterialParamChanged(Material* baseMaterial, const jstringID& paramName);
        void onBaseMaterialClear(Material* baseMaterial) { clearMaterial(); }

        bool isEngineInternalMaterialParam(const jstringID& name) const;
        void onEngineInternalParamChanged(ShadersSubsystem* subsystem, const jstringID& internalParamName);
    };
}
