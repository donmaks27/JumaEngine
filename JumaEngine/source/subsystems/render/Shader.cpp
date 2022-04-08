// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Shader.h"

#include "RenderSubsystem.h"
#include "engine/Engine.h"

namespace JumaEngine
{
    Shader::~Shader()
    {
        clearData();
    }

    bool Shader::init(const jstring& vertexShader, const jstring& fragmentShader, jmap<jstringID, ShaderUniform> uniforms)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Shader already initalized"));
            return false;
        }
        if (vertexShader.isEmpty() || fragmentShader.isEmpty())
        {
            JUMA_LOG(warning, JSTR("Empty shader name"));
            return false;
        }

        m_VertexShaderName = vertexShader;
        m_FragmentShaderName = fragmentShader;
        m_ShaderUniforms = std::move(uniforms);

        markAsInitialized();
        return true;
    }

    Shader_RenderAPIObject* Shader::createRenderAPIObjectInternal()
    {
        return getOwnerEngine()->getRenderSubsystem()->createShaderObject();
    }

    void Shader::clearData()
    {
        onClear.call(this);

        clearRenderAPIObject();

        m_VertexShaderName.clear();
        m_FragmentShaderName.clear();
        m_ShaderUniforms.clear();
    }
}
