// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../EngineSubsystem.h"

#include <jutils/jlist.h>

#include "Material.h"
#include "Shader.h"

namespace JumaEngine
{
    class ShadersSubsystem final : public EngineSubsystem
    {
        JUMAENGINE_CLASS(ShadersSubsystem, EngineSubsystem)

    public:
        ShadersSubsystem() = default;
        virtual ~ShadersSubsystem() override;

        Shader* getShader(const jstringID& shaderName);

        Material* createMaterial(Shader* shader);
        Material* createMaterial(Material* baseMaterial);
        void destroyMaterial(Material* material);

        void clear();

    private:

        jmap<jstringID, Shader> m_Shaders;
        jlist<Material> m_Materials;

        jarray<const Material*> m_DestroyingMaterials;


        void onMaterialClear(Material* material);
    };
}
