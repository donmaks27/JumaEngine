﻿// Copyright © 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../../EngineContextObject.h"

#include <JumaRE/material/Shader.h>

namespace JumaEngine
{
    class ShadersSubsystem;

    class Shader final : public EngineContextObject
    {
        JUMAENGINE_CLASS(Shader, EngineContextObject)

        friend ShadersSubsystem;

    public:
        Shader() = default;
        virtual ~Shader() override = default;

        const jstringID& getName() const { return m_ShaderName; }
        const jmap<jstringID, jstringID>& getEngineInternalParamNames() const { return m_EngineInternalParamNames; }

        JumaRE::Shader* getShader() const { return m_Shader; }

    private:

        JumaRE::Shader* m_Shader = nullptr;
        jstringID m_ShaderName = jstringID_NONE;

        jmap<jstringID, jstringID> m_EngineInternalParamNames; // Material param name - internal param name


        bool loadShader(const jstringID& shaderName);
        void clearShader();
    };
}