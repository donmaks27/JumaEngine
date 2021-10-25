// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Shader.h"

#include "utils/jlog.h"

namespace JumaEngine
{
    bool Shader::init(const jstring& shaderName, const jarray<ShaderUniform>& uniforms)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Shader already loaded"));
            return false;
        }
        if (shaderName.empty())
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

    void Shader::clear()
    {
        if (isValid())
        {
            clearInternal();
            m_Initialized = false;
        }
    }
}
