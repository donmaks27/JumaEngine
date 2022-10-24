// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../EngineSubsystem.h"

#include <jutils/jlist.h>

#include "Material.h"
#include "Shader.h"

namespace JumaEngine
{
    class ShadersSubsystem;

    JUTILS_CREATE_MULTICAST_DELEGATE2(OnShadersSubsystemMaterialParamEvent, ShadersSubsystem*, subsystem, const jstringID&, paramName);

    class ShadersSubsystem final : public EngineSubsystem
    {
        JUMAENGINE_CLASS(ShadersSubsystem, EngineSubsystem)

    public:
        ShadersSubsystem() = default;
        virtual ~ShadersSubsystem() override;

        OnShadersSubsystemMaterialParamEvent onEngineInternalParamChanged;


        Shader* getShader(const jstringID& shaderName);

        Material* createMaterial(Shader* shader);
        Material* createMaterial(Material* baseMaterial);
        void destroyMaterial(Material* material);

        template<JumaRE::ShaderUniformType Type>
        bool setParamValue(const jstringID& name, const typename JumaRE::ShaderUniformInfo<Type>::value_type& value)
        {
            if (!m_EngineInternalParams.contains(name, Type) || !m_EngineInternalParams.setValue<Type>(name, value))
            {
                return false;
            }
            onEngineInternalParamChanged.call(this, name);
            return true;
        }
        template<JumaRE::ShaderUniformType Type>
        bool getParamValue(const jstringID& name, typename JumaRE::ShaderUniformInfo<Type>::value_type& outValue) const
        {
            return m_EngineInternalParams.getValue<Type>(name, outValue);
        }
        const JumaRE::MaterialParamsStorage& getEngineInternalParams() const { return m_EngineInternalParams; }

        void clear();

    protected:

        virtual bool initSubsystem() override;

    private:

        JumaRE::MaterialParamsStorage m_EngineInternalParams;

        jmap<jstringID, Shader> m_Shaders;
        jlist<Material> m_Materials;

        jarray<const Material*> m_DestroyingMaterials;


        void onMaterialClear(Material* material);
    };
}
