// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineContextObject.h"

#include <JumaRE/material/Material.h>
#include <jutils/jdelegate_multicast.h>

#include "MaterialParamType.h"

namespace JumaEngine
{
    class AssetsEngineSubsystem;
    class Material;
    class Shader;

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

        template<MaterialParamType Type>
        bool setParamValue(const jstringID& name, const typename MaterialParamInfo<Type>::value_type& value);
        template<>
        bool setParamValue<MaterialParamType::Texture>(const jstringID& name, const EngineObjectPtr<TextureBase>& value);
        bool resetParamValue(const jstringID& name);
        template<MaterialParamType Type>
        bool getParamValue(const jstringID& name, typename MaterialParamInfo<Type>::value_type& outValue) const;

    protected:

        virtual void onObjectDescriptorDestroying() override;

    private:
        
        JUTILS_CREATE_MULTICAST_DELEGATE1(OnMaterialParamEvent, const jstringID&, paramName);
        
        OnMaterialParamEvent onParamChanged;

        EngineObjectPtr<Shader> m_BaseShader = nullptr;
        EngineObjectPtr<Material> m_BaseMaterial = nullptr;

        JumaRE::Material* m_Material = nullptr;
        jset<jstringID> m_OverridedParams;
        jmap<jstringID, EngineObjectPtr<TextureBase>> m_ReferencedTextures;


        bool createMaterial(const EngineObjectPtr<Shader>& shader);
        bool createMaterial(const EngineObjectPtr<Material>& baseMaterial);
        void clearMaterial();

        void onBaseMaterialParamChanged(const jstringID& paramName);

        bool isGlobalMaterialParam(const jstringID& name) const;
        void onGlobalParamChanged(AssetsEngineSubsystem* subsystem, const jstringID& internalParamName);
    };

    template<MaterialParamType Type>
    bool Material::setParamValue(const jstringID& name, const typename MaterialParamInfo<Type>::value_type& value)
    {
        if (isGlobalMaterialParam(name) || !m_Material->setParamValue<Type>(name, value))
        {
            return false;
        }
        m_OverridedParams.add(name);
        onParamChanged.call(name);
        return true;
    }

    template<MaterialParamType Type>
    bool Material::getParamValue(const jstringID& name, typename MaterialParamInfo<Type>::value_type& outValue) const
    {
        return m_Material->getParamValue(name, outValue);
    }
}
