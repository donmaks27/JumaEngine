// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Shader.h"

#include "RenderSubsystem.h"
#include "engine/Engine.h"

namespace JumaEngine
{
    Shader::~Shader()
    {
        clear();
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

        m_Initialized = true;
        return true;
    }
    void Shader::clear()
    {
        if (isValid())
        {
            onClear.call(this);

            clearRenderObject();

            m_VertexShaderName.clear();
            m_FragmentShaderName.clear();
            m_ShaderUniforms.clear();

            m_Initialized = false;
        }
    }

    bool Shader::createRenderObject()
    {
        if (!isValid())
        {
            JUMA_LOG(warning, JSTR("Shader not initialized"));
            return false;
        }

        RenderSubsystem* renderSubsystem = getOwnerEngine()->getRenderSubsystem();
        m_RenderObject = renderSubsystem->createShaderObject();
        if (!m_RenderObject->init(this))
        {
            delete m_RenderObject;
            m_RenderObject = nullptr;
            return false;
        }
        return true;
    }
    void Shader::clearRenderObject()
    {
        if (m_RenderObject != nullptr)
        {
            delete m_RenderObject;
            m_RenderObject = nullptr;
        }
    }
}
