// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "ShaderBase.h"
#include "utils/log.h"

namespace JumaEngine
{
    // TODO: Need to recreate map on windows count changes, in that case i don't need mutex anymore
    jmutex_shared ShaderBase::s_ActiveShadersMutex = jmutex_shared();
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

            s_ActiveShadersMutex.lock();
            s_ActiveShaders.add(windowID, this);
            s_ActiveShadersMutex.unlock();
        }
    }
    void ShaderBase::deactivate(const window_id windowID)
    {
        if (isActive(windowID))
        {
            deactivateShaderInternal();

            s_ActiveShadersMutex.lock_shared();
            s_ActiveShaders.add(windowID, nullptr);
            s_ActiveShadersMutex.unlock_shared();
        }
    }

    ShaderBase* ShaderBase::getActiveShader(const window_id windowID)
    {
        std::shared_lock lock(s_ActiveShadersMutex);
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
