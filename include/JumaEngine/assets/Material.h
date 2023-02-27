// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineContextObject.h"

#include <JumaRE/material/Material.h>
#include <jutils/jdelegate_multicast.h>

#include "Shader.h"

namespace JumaEngine
{
    class Material;
    class AssetsEngineSubsystem;

    using ShaderUniformType = JumaRE::ShaderUniformType;

    JUTILS_CREATE_MULTICAST_DELEGATE1(OnMaterialEvent, Material*, material);
    JUTILS_CREATE_MULTICAST_DELEGATE2(OnMaterialParamEvent, Material*, material, const jstringID&, paramName);

    class Material final : public EngineContextObject
    {
        JUMAENGINE_CLASS(Material, EngineContextObject)

        friend AssetsEngineSubsystem;

    public:
        Material() = default;
        virtual ~Material() override = default;

        OnMaterialEvent onClear;
        OnMaterialParamEvent onParamChanged;


        Material* getBaseMaterial() const { return m_BaseMaterial; }
        EngineObjectPtr<Shader> getBaseShader() const { return m_BaseShader != nullptr ? m_BaseShader : (m_BaseMaterial != nullptr ? m_BaseMaterial->getBaseShader() : nullptr); }

        JumaRE::Material* getMaterial() const { return m_Material; }
        JumaRE::Shader* getShader() const { return m_Material != nullptr ? m_Material->getShader() : nullptr; }

        template<ShaderUniformType Type>
        bool setParamValue(const jstringID& name, const typename JumaRE::ShaderUniformInfo<Type>::value_type& value)
        {
            if (isGlobalMaterialParam(name) || !m_Material->setParamValue<Type>(name, value))
            {
                return false;
            }
            onParamChanged.call(this, name);
            return true;
        }
        bool resetParamValue(const jstringID& name)
        {
            if (isGlobalMaterialParam(name) || !m_Material->resetParamValue(name))
            {
                return false;
            }
            onParamChanged.call(this, name);
            return true;
        }
        template<ShaderUniformType Type>
        bool getParamValue(const jstringID& name, typename JumaRE::ShaderUniformInfo<Type>::value_type& outValue) const { return m_Material->getParamValue(name, outValue); }

    private:

        EngineObjectPtr<Shader> m_BaseShader = nullptr;
        Material* m_BaseMaterial = nullptr;

        JumaRE::Material* m_Material = nullptr;


        bool createMaterial(const EngineObjectPtr<Shader>& shader);
        bool createMaterial(Material* baseMaterial);
        void clearMaterial();

        void onBaseMaterialParamChanged(Material* baseMaterial, const jstringID& paramName);
        void onBaseMaterialClear(Material* baseMaterial) { clearMaterial(); }

        bool isGlobalMaterialParam(const jstringID& name) const;
        void onGlobalParamChanged(AssetsEngineSubsystem* subsystem, const jstringID& internalParamName);
    };
}
