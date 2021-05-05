// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "ShaderBase.h"
#include "utils/log.h"

namespace JumaEngine
{
    bool ShaderBase::loadShader(const jstring& shaderName)
    {
        if (isShaderLoaded())
        {
            JUMA_LOG(warning, JTEXT("Shader already loaded."));
            return false;
        }
        if (shaderName.empty())
        {
            JUMA_LOG(warning, JTEXT("Empty shader name."));
            return false;
        }

        if (!loadShaderInternal(shaderName))
        {
            JUMA_LOG(warning, jstring(JTEXT("Fail to load shader ")) + shaderName + JTEXT("."));
            return false;
        }

        m_ShaderName = shaderName;
        m_ShaderLoaded = true;
        return true;
    }
    void ShaderBase::clearShader()
    {
        if (isShaderLoaded())
        {
            clearShaderInternal();
        }
    }

    bool ShaderBase::activate()
    {
        if (isShaderLoaded())
        {
            activateShaderInternal();
            return true;
        }
        return false;
    }
    void ShaderBase::deactivate()
    {
        deactivateShaderInternal();
    }
}
