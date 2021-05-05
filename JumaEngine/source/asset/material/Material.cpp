// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Material.h"
#include "render/shader/ShaderBase.h"

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

    void Material::cacheShaderUniformName(const jstring& name) const
    {
        if (isShaderValid())
        {
            m_Shader->cacheShaderUniformName(*name);
        }
    }

	bool Material::activate() const
	{
        return isInit() ? m_Shader->activate() : false;
	}
	void Material::deactivate() const
	{
		if (isInit())
        {
            m_Shader->deactivate();
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
}
