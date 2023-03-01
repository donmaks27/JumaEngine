// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineContextObject.h"

#include <JumaRE/material/Material.h>
#include <jutils/jdelegate_multicast.h>

namespace JumaEngine
{
    class AssetsEngineSubsystem;
    class Material;
    class Shader;

    using ShaderUniformType = JumaRE::ShaderUniformType;

    class Material final : public EngineContextObject
    {
        JUMAENGINE_CLASS(Material, EngineContextObject)

        friend AssetsEngineSubsystem;

    public:
        Material() = default;
        virtual ~Material() override = default;
        
        const EngineObjectPtr<Material>& getBaseMaterial() const { return m_BaseMaterial; }
        const EngineObjectPtr<Shader>& getBaseShader() const { return m_BaseShader != nullptr ? m_BaseShader : (m_BaseMaterial != nullptr ? m_BaseMaterial->getBaseShader() : m_BaseShader); }

        JumaRE::Material* getMaterial() const { return m_Material; }
        JumaRE::Shader* getShader() const { return m_Material != nullptr ? m_Material->getShader() : nullptr; }

        template<ShaderUniformType Type>
        bool setParamValue(const jstringID& name, const typename JumaRE::ShaderUniformInfo<Type>::value_type& value)
        {
            if (isGlobalMaterialParam(name) || !m_Material->setParamValue<Type>(name, value))
            {
                return false;
            }
            onParamChanged.call(name);
            return true;
        }
        bool resetParamValue(const jstringID& name)
        {
            if (isGlobalMaterialParam(name) || !m_Material->resetParamValue(name))
            {
                return false;
            }
            onParamChanged.call(name);
            return true;
        }
        template<ShaderUniformType Type>
        bool getParamValue(const jstringID& name, typename JumaRE::ShaderUniformInfo<Type>::value_type& outValue) const { return m_Material->getParamValue(name, outValue); }

    protected:

        virtual void onObjectDescriptorDestroying() override;

    private:
        
        JUTILS_CREATE_MULTICAST_DELEGATE1(OnMaterialParamEvent, const jstringID&, paramName);
        
        OnMaterialParamEvent onParamChanged;

        EngineObjectPtr<Shader> m_BaseShader = nullptr;
        EngineObjectPtr<Material> m_BaseMaterial = nullptr;

        JumaRE::Material* m_Material = nullptr;


        bool createMaterial(const EngineObjectPtr<Shader>& shader);
        bool createMaterial(const EngineObjectPtr<Material>& baseMaterial);
        void clearMaterial();

        void onBaseMaterialParamChanged(const jstringID& paramName);

        bool isGlobalMaterialParam(const jstringID& name) const;
        void onGlobalParamChanged(AssetsEngineSubsystem* subsystem, const jstringID& internalParamName);
    };
}
