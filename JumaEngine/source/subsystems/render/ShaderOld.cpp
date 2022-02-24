// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "ShaderOld.h"
#include "jutils/jlog.h"

namespace JumaEngine
{
    ShaderOld::~ShaderOld()
    {
        if (isValid())
        {
            onShaderCleared();
        }
    }

    bool ShaderOld::init(const jstring& shaderName, const jmap<jstring, ShaderUniform>& uniforms)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Shader already loaded"));
            return false;
        }
        if (shaderName.isEmpty())
        {
            JUMA_LOG(warning, JSTR("Shader name is empty"));
            return false;
        }
        if (!initInternal(shaderName, uniforms))
        {
            return false;
        }

        m_Initialized = true;
        m_Name = shaderName;
        m_ShaderUniforms = uniforms;
        return true;
    }

    void ShaderOld::clear()
    {
        if (isValid())
        {
            onShaderPreClear();

            clearInternal();
            m_Initialized = false;

            onShaderCleared();
        }
    }
}
