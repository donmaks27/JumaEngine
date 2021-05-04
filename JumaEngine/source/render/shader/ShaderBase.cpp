// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "ShaderBase.h"

#include "render/RenderManagerBase.h"
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

    void ShaderBase::activate(const window_id windowID)
    {
        if (isShaderLoaded())
        {
            RenderManagerBase* renderManager = getRenderManager();
            if (renderManager != nullptr)
            {
                renderManager->setWindowActiveShader(windowID, this);
                activateShaderInternal();
            }
        }
    }
    bool ShaderBase::isActive(const window_id windowID) const
    {
        RenderManagerBase* renderManager = getRenderManager();
        return (renderManager != nullptr) && (renderManager->getWindowActiveShader(windowID) == this);
    }
    void ShaderBase::deactivate(const window_id windowID)
    {
        RenderManagerBase* renderManager = getRenderManager();
        if (renderManager != nullptr)
        {
            deactivateShaderInternal();
            renderManager->setWindowActiveShader(windowID, nullptr);
        }
    }
}
