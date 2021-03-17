// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "ShaderBase.h"
#include "utils/log.h"

namespace JumaEngine
{
    ShaderBase* ShaderBase::s_ActiveShader = nullptr;

    ShaderBase::ShaderBase()
    {
    }
    ShaderBase::~ShaderBase()
    {
    }

    bool ShaderBase::loadShader(const std::string& shaderName)
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

        loadShaderInternal(shaderName);
        return isShaderLoaded();
    }
    void ShaderBase::clearShader()
    {
        deactivateShader();
        if (isShaderLoaded())
        {
            clearShaderInternal();
        }
    }

    void ShaderBase::activateShader()
    {
        if (!isShaderActive() && isShaderLoaded())
        {
            if (m_ShouldAlwaysDeactivateOldShader)
            {
                deactivateActiveShader();
            }

            activateShaderInternal();
            s_ActiveShader = this;
        }
    }
    void ShaderBase::deactivateShader()
    {
        if (isShaderActive())
        {
            deactivateShaderInternal();
            s_ActiveShader = nullptr;
        }
    }

    void ShaderBase::deactivateActiveShader()
    {
        if (hasActiveShader())
        {
            s_ActiveShader->deactivateShader();
        }
    }
}
