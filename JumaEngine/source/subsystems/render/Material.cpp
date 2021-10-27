// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Material.h"
#include "Shader.h"
#include "utils/jlog.h"
#include "MaterialUniform.h"

namespace JumaEngine
{
    Material::~Material()
    {
        if (isValid())
        {
            clearMaterial();
            onMaterialCleared();
        }
    }

    bool Material::isValid() const
    {
        return isMaterialInstance() ? m_BaseMaterial->isValid() : (m_BaseShader != nullptr) && m_BaseShader->isValid();
    }

    bool Material::init(const jshared_ptr<Shader>& shader)
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

        initUniformValues(shader);
        if (!initInternal(shader))
        {
            clearUniformValues();
            return false;
        }

        m_BaseShader = shader;
        m_BaseShader->onPreClear.bind(this, &Material::onBaseShaderPreClear);
        return true;
    }
    bool Material::init(const jshared_ptr<Material>& material)
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

        m_BaseMaterial->onPreClear.bind(this, &Material::onBaseMaterialPreClear);
        return true;
    }

    void Material::initUniformValues(const jshared_ptr<Shader>& shader)
    {
        for (const auto& nameAndShaderUniform : shader->getUniforms())
        {
            MaterialUniform* uniform = MaterialUniformActions::create(nameAndShaderUniform.second.type);
            if (uniform != nullptr)
            {
                m_UniformNames.add(nameAndShaderUniform.first);
                m_UniformValues.add(uniform);
            }
        }
    }

    void Material::clear()
    {
        if (isValid())
        {
            onMaterialPreClear();

            clearInternal();
            clearMaterial();

            onMaterialCleared();
        }
    }
    void Material::clearMaterial()
    {
        clearUniformValues();
        if (m_BaseShader != nullptr)
        {
            m_BaseShader->onPreClear.unbind(this, &Material::onBaseShaderPreClear);
            m_BaseShader = nullptr;
        }
        if (m_BaseMaterial != nullptr)
        {
            m_BaseMaterial->onPreClear.unbind(this, &Material::onBaseMaterialPreClear);
            m_BaseMaterial = nullptr;
        }
    }
    void Material::clearUniformValues()
    {
        for (const auto& uniform : m_UniformValues)
        {
            MaterialUniformActions::clear(uniform);
        }
        m_UniformNames.clear();
        m_UniformValues.clear();
    }

    ShaderUniformType Material::getUniformType(const jstring& name) const
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
    MaterialUniform* Material::getUniformValue(const jstring& name) const
    {
        if (isValid())
        {
            const int64 index = m_UniformNames.indexOf(name);
            if (index != -1)
            {
                return m_UniformValues[index];
            }
            if (isMaterialInstance() && getUniformNames().contains(name))
            {
                return m_BaseMaterial->getUniformValue(name);
            }
        }
        return nullptr;
    }
}
