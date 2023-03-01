// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "../engine/EngineContextObject.h"

#include <JumaRE/material/Shader.h>

namespace JumaEngine
{
    class AssetsEngineSubsystem;

    class Shader final : public EngineContextObject
    {
        JUMAENGINE_CLASS(Shader, EngineContextObject)

        friend AssetsEngineSubsystem;

    public:
        Shader() = default;
        virtual ~Shader() override = default;

        const jstringID& getName() const { return m_ShaderName; }
        const jmap<jstringID, jstringID>& getEngineInternalParamNames() const { return m_EngineInternalParamNames; }

        JumaRE::Shader* getShader() const { return m_Shader; }

    protected:

        virtual void onObjectDescriptorDestroying() override;

    private:

        JumaRE::Shader* m_Shader = nullptr;
        jstringID m_ShaderName = jstringID_NONE;

        jmap<jstringID, jstringID> m_EngineInternalParamNames;


        bool loadShader(const jstringID& shaderName, const jstring& contentFolder);
        void clearShader();
    };
}
