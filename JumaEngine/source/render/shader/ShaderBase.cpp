// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "ShaderBase.h"
#include "utils/log.h"

namespace JumaEngine
{
    jmap<window_id, ShaderBase*> ShaderBase::s_ActiveShaders = {};

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

    void ShaderBase::activate(const window_id windowID)
    {
        if (isShaderLoaded() && !isActive(windowID))
        {
            activateShaderInternal();
            s_ActiveShaders.add(windowID, this);
        }
    }
    void ShaderBase::deactivate(const window_id windowID)
    {
        if (isActive(windowID))
        {
            deactivateShaderInternal();
            s_ActiveShaders.add(windowID, nullptr);
        }
    }

    ShaderBase* ShaderBase::getActiveShader(const window_id windowID)
    {
        ShaderBase** shaderPtr = s_ActiveShaders.findByKey(windowID);
        return shaderPtr != nullptr ? *shaderPtr : nullptr;
    }
    void ShaderBase::deactivateActiveShader(const window_id windowID)
    {
        ShaderBase* shader = getActiveShader(windowID);
        if (shader != nullptr)
        {
            shader->deactivate(windowID);
        }
    }
}
