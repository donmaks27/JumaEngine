// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MaterialOld.h"
#include "render/shader/ShaderBase.h"

namespace JumaEngine
{
	MaterialOld::~MaterialOld()
	{
		clearShader();
	}

	bool MaterialOld::isShaderValid() const
	{
		return (m_Shader != nullptr) && m_Shader->isShaderLoaded();
	}
	jstring MaterialOld::getShaderName() const
	{
		return m_Shader != nullptr ? m_Shader->getShaderName() : jstring();
	}

    void MaterialOld::cacheShaderUniformName(const jstring& name) const
    {
        if (isShaderValid())
        {
            m_Shader->cacheShaderUniformName(*name);
        }
    }

	bool MaterialOld::activate() const
	{
        return isInit() ? m_Shader->activate() : false;
	}
	void MaterialOld::deactivate() const
	{
		if (isInit())
        {
            m_Shader->deactivate();
        }
	}

	void MaterialOld::clearShader()
	{
		if (m_Shader != nullptr)
        {
            delete m_Shader;
            m_Shader = nullptr;
        }
	}
}
