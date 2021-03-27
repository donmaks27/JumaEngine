// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MaterialBase.h"
#include "render/shader/ShaderBase.h"
#include "utils/system_functions.h"

namespace JumaEngine
{
    MaterialBase::~MaterialBase()
    {
        clearShader();
    }

    bool MaterialBase::isShaderValid() const
    {
        return (m_Shader != nullptr) && m_Shader->isShaderLoaded();
    }
    jstring MaterialBase::getShaderName() const
    {
        return m_Shader != nullptr ? m_Shader->getShaderName() : jstring();
    }
    void MaterialBase::setShaderName(const jstring& shaderName)
    {
        if (m_Shader == nullptr)
        {
            m_Shader = SystemFunctions::createShader(this, shaderName);
        }
        else if (m_Shader->getShaderName() != shaderName)
        {
            m_Shader->clearShader();
            if (!shaderName.empty())
            {
                m_Shader->loadShader(shaderName);
            }
        }
    }
    void MaterialBase::clearShader()
    {
        if (m_Shader != nullptr)
        {
            delete m_Shader;
            m_Shader = nullptr;
        }
    }

    void MaterialBase::activate() const
    {
        if (isShaderValid() && !m_Shader->isShaderActive())
        {
            m_Shader->activateShader();
            if (m_Shader->isShaderActive())
            {
                loadShaderParams();
            }
        }
    }
    bool MaterialBase::isActive() const
    {
        return isShaderValid() && m_Shader->isShaderActive();
    }
    void MaterialBase::deactivate() const
    {
        if (isActive())
        {
            m_Shader->deactivateShader();
        }
    }
}
