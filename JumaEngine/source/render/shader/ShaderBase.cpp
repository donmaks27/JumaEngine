// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "ShaderBase.h"
#include "utils/log.h"

namespace JumaEngine
{
    ShaderBase* ShaderBase::s_ActiveShader = nullptr;

    bool ShaderBase::loadShader(const jstring& shaderName)
    {
        if (isShaderLoaded())
        {
            JUMA_LOG(warning, JTEXT("Shader already loaded!"));
            return false;
        }
        if (shaderName.empty())
        {
            JUMA_LOG(warning, JTEXT("Empty shader name!"));
            return false;
        }
        if (shaderName == m_ShaderName)
        {
            JUMA_LOG(warning, JTEXT("Shader name is the same as old one!"));
            return false;
        }

        loadShaderInternal(shaderName);
        if (isShaderLoaded())
        {
            m_ShaderName = shaderName;
            return true;
        }
        return false;
    }
    void ShaderBase::clearShader()
    {
        deactivate();
        if (isShaderLoaded())
        {
            clearShaderInternal();
        }
    }

    void ShaderBase::activate()
    {
        if (!isActive() && isShaderLoaded())
        {
            activateShaderInternal();
            s_ActiveShader = this;
        }
    }
    void ShaderBase::deactivate()
    {
        if (isActive())
        {
            deactivateShaderInternal();
            clearActiveShaderRef();
        }
    }

    void ShaderBase::deactivateActiveShader()
    {
        if (hasActiveShader())
        {
            s_ActiveShader->deactivate();
        }
    }
}
