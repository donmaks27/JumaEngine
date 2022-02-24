// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MaterialOld.h"
#include "ShaderOld.h"
#include "jutils/jlog.h"
#include "MaterialUniform.h"

namespace JumaEngine
{
    MaterialOld::~MaterialOld()
    {
        if (isValid())
        {
            clearMaterial();
            onMaterialCleared();
        }
    }

    bool MaterialOld::isValid() const
    {
        return isMaterialInstance() ? m_BaseMaterial->isValid() : (m_BaseShader != nullptr) && m_BaseShader->isValid();
    }

    bool MaterialOld::init(const jshared_ptr<ShaderOld>& shader)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Material already initialized"));
            return false;
        }
        if ((shader == nullptr) || !shader->isValid())
        {
            JUMA_LOG(warning, JSTR("Shader not valid"));
            return false;
        }

        m_BaseShader = shader;
        initUniformValues(shader);
        if (!initInternal(shader))
        {
            clearUniformValues();
            m_BaseShader = nullptr;
            return false;
        }

        m_BaseShader->onPreClear.bind(this, &MaterialOld::onBaseShaderPreClear);
        return true;
    }
    bool MaterialOld::init(const jshared_ptr<MaterialOld>& material)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Material already initialized"));
            return false;
        }
        if ((material == nullptr) || !material->isValid())
        {
            JUMA_LOG(warning, JSTR("Base material not valid"));
            return false;
        }

        m_BaseMaterial = material;
        if (!initInternal(material->getShader()))
        {
            m_BaseMaterial = nullptr;
            return false;
        }

        m_BaseMaterial->onPreClear.bind(this, &MaterialOld::onBaseMaterialPreClear);
        return true;
    }

    void MaterialOld::initUniformValues(const jshared_ptr<ShaderOld>& shader)
    {
        for (const auto& nameAndShaderUniform : shader->getUniforms())
        {
            MaterialUniform* uniform = MaterialUniformActions::create(nameAndShaderUniform.value.type);
            if (uniform != nullptr)
            {
                m_UniformNames.add(nameAndShaderUniform.key);
                m_UniformValues.add(uniform);
            }
        }
    }

    void MaterialOld::clear()
    {
        if (isValid())
        {
            onMaterialPreClear();

            clearInternal();
            clearMaterial();

            onMaterialCleared();
        }
    }
    void MaterialOld::clearMaterial()
    {
        clearUniformValues();
        if (m_BaseShader != nullptr)
        {
            m_BaseShader->onPreClear.unbind(this, &MaterialOld::onBaseShaderPreClear);
            m_BaseShader = nullptr;
        }
        if (m_BaseMaterial != nullptr)
        {
            m_BaseMaterial->onPreClear.unbind(this, &MaterialOld::onBaseMaterialPreClear);
            m_BaseMaterial = nullptr;
        }
    }
    void MaterialOld::clearUniformValues()
    {
        for (const auto& uniform : m_UniformValues)
        {
            MaterialUniformActions::terminate(uniform);
        }
        m_UniformNames.clear();
        m_UniformValues.clear();
    }

    ShaderUniformType MaterialOld::getUniformType(const jstring& name) const
    {
        if (isValid())
        {
            const ShaderUniform* uniform = getShader()->getUniforms().find(name);
            if (uniform != nullptr)
            {
                return uniform->type;
            }
        }
        return ShaderUniformType::None;
    }
    MaterialUniform* MaterialOld::getUniformValue(const jstring& name) const
    {
        const int32 index = m_UniformNames.indexOf(name);
        if (index != -1)
        {
            return m_UniformValues[index];
        }
        if (isMaterialInstance() && getUniformNames().contains(name))
        {
            return m_BaseMaterial->getUniformValue(name);
        }
        return nullptr;
    }
}
