// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Material.h"
#include "render/shader/ShaderBase.h"
#include "utils/log.h"
#include "utils/system_functions.h"

namespace JumaEngine
{
	Material::~Material()
	{
		clearShader();
	}

	bool Material::isShaderValid() const
	{
		return (m_Shader != nullptr) && m_Shader->isShaderLoaded();
	}
	jstring Material::getShaderName() const
	{
		return m_Shader != nullptr ? m_Shader->getShaderName() : jstring();
	}
	void Material::setShaderName(const jstring& shaderName)
	{
		if (m_Initialized)
		{
			JUMA_LOG(warning, jstring(JTEXT("Material ")) + getShaderName() + JTEXT(" already initialized!"));
		}
		else if (m_Shader == nullptr)
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
	void Material::clearShader()
	{
		if (m_Shader != nullptr)
        {
            delete m_Shader;
            m_Shader = nullptr;
        }
	}

	void Material::activate() const
	{
		if (isInit())
		{
			m_Shader->activate();
		}
	}
	bool Material::isActive() const
	{
		return isInit() && m_Shader->isActive();
	}
	void Material::deactivate() const
	{
		if (isActive())
        {
            m_Shader->deactivate();
        }
	}
}
