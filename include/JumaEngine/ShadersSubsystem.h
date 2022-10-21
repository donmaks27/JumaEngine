// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "EngineSubsystem.h"

#include <JumaRE/material/Shader.h>

namespace JumaEngine
{
    class ShadersSubsystem : public EngineSubsystem
    {
        JUMAENGINE_CLASS(ShadersSubsystem, EngineSubsystem)

    public:
        ShadersSubsystem() = default;
        virtual ~ShadersSubsystem() override;

        JumaRE::Shader* getShader(const jstring& shaderName);
        void clear();

    private:

        jmap<jstring, JumaRE::Shader*> m_Shaders;
    };
}
