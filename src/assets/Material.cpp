// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/Material.h"

#include "JumaEngine/engine/Engine.h"
#include "JumaEngine/assets/AssetsEngineSubsystem.h"

namespace JumaEngine
{
    template<JumaRE::ShaderUniformType Type>
    bool CopyMaterialParamTemplate(JumaRE::Material* material, const jstringID& writeName, const JumaRE::MaterialParamsStorage& materialParams, const jstringID& readName)
    {
        typename JumaRE::ShaderUniformInfo<Type>::value_type value;
        if (materialParams.getValue<Type>(readName, value))
        {
            return material->setParamValue<Type>(writeName, value);
        }
        return false;
    }
    inline bool CopyMaterialParam(const JumaRE::ShaderUniformType type, JumaRE::Material* material, const jstringID& writeName, const JumaRE::MaterialParamsStorage& materialParams, const jstringID& readName)
    {
        switch (type)
        {
        case JumaRE::ShaderUniformType::Float:
            return CopyMaterialParamTemplate<JumaRE::ShaderUniformType::Float>(material, writeName, materialParams, readName);
        case JumaRE::ShaderUniformType::Vec2:
            return CopyMaterialParamTemplate<JumaRE::ShaderUniformType::Vec2>(material, writeName, materialParams, readName);
        case JumaRE::ShaderUniformType::Vec4:
            return CopyMaterialParamTemplate<JumaRE::ShaderUniformType::Vec4>(material, writeName, materialParams, readName);
        case JumaRE::ShaderUniformType::Mat4:
            return CopyMaterialParamTemplate<JumaRE::ShaderUniformType::Mat4>(material, writeName, materialParams, readName);
        case JumaRE::ShaderUniformType::Texture:
            return CopyMaterialParamTemplate<JumaRE::ShaderUniformType::Texture>(material, writeName, materialParams, readName);
        default: ;
        }
        return false;
    }
    inline bool CopyMaterialParam(const JumaRE::ShaderUniformType type, JumaRE::Material* material, const JumaRE::MaterialParamsStorage& materialParams, const jstringID& name)
    {
        return CopyMaterialParam(type, material, name, materialParams, name);
    }
    inline bool CopyMaterialParam(JumaRE::Material* material, const jstringID& writeName, const JumaRE::MaterialParamsStorage& materialParams, const jstringID& readName)
    {
        const JumaRE::ShaderUniform* uniform = material->getShader()->getUniforms().find(writeName);
        if (uniform != nullptr)
        {
            return CopyMaterialParam(uniform->type, material, writeName, materialParams, readName);
        }
        return false;
    }
    inline bool CopyMaterialParam(JumaRE::Material* material, const JumaRE::MaterialParamsStorage& materialParams, const jstringID& name)
    {
        const JumaRE::ShaderUniform* uniform = material->getShader()->getUniforms().find(name);
        if (uniform != nullptr)
        {
            return CopyMaterialParam(uniform->type, material, materialParams, name);
        }
        return false;
    }
    
    bool Material::createMaterial(const EngineObjectPtr<Shader>& shader)
    {
        if (shader == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invalid input shader"));
            return false;
        }

        const Engine* engine = getEngine();
        JumaRE::Shader* shaderObject = shader->getShader();
        JumaRE::Material* materialObject = engine->getRenderEngine()->createMaterial(shaderObject);
        if (materialObject == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create material"));
            return false;
        }

        m_Material = materialObject;
        m_BaseShader = shader;
        
	    const jmap<jstringID, jstringID>& shaderParamNames = getBaseShader()->getGlobalParamNames();
        if (!shaderParamNames.isEmpty())
        {
	        AssetsEngineSubsystem* assetsSubsystem = engine->getSubsystem<AssetsEngineSubsystem>();
	        const JumaRE::MaterialParamsStorage& globalParams = assetsSubsystem->getGlobalParams();
	        for (const auto& uniform : shaderObject->getUniforms())
	        {
	            const jstringID* globalParamName = shaderParamNames.find(uniform.key);
	            if (globalParamName != nullptr)
	            {
	                CopyMaterialParam(uniform.value.type, materialObject, uniform.key, globalParams, *globalParamName);
	            }
	        }
            assetsSubsystem->onEngineInternalParamChanged.bind(this, &Material::onGlobalParamChanged);
        }
        return true;
    }
    bool Material::createMaterial(const EngineObjectPtr<Material>& baseMaterial)
    {
        if (baseMaterial == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invalid input material"));
            return false;
        }

        Engine* engine = getEngine();
        const JumaRE::Material* baseMaterialObject = baseMaterial->getMaterial();
        JumaRE::Shader* shaderObject = baseMaterialObject->getShader();
        JumaRE::Material* materialObject = engine->getRenderEngine()->createMaterial(shaderObject);
        if (materialObject == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create material"));
            return false;
        }

        m_Material = materialObject;
        m_BaseMaterial = baseMaterial;

        AssetsEngineSubsystem* assetsSubsystem = engine->getSubsystem<AssetsEngineSubsystem>();
        const JumaRE::MaterialParamsStorage& globalParams = assetsSubsystem->getGlobalParams();
        const JumaRE::MaterialParamsStorage& baseMaterialParams = baseMaterialObject->getMaterialParams();
        const jmap<jstringID, jstringID>& shaderParamNames = getBaseShader()->getGlobalParamNames();
        for (const auto& uniform : shaderObject->getUniforms())
        {
            const jstringID* globalParamName = shaderParamNames.find(uniform.key);
            if (globalParamName != nullptr)
            {
                CopyMaterialParam(uniform.value.type, materialObject, uniform.key, globalParams, *globalParamName);
            }
            else
            {
                CopyMaterialParam(uniform.value.type, materialObject, baseMaterialParams, uniform.key);
            }
        }

        if (!shaderParamNames.isEmpty())
        {
            assetsSubsystem->onEngineInternalParamChanged.bind(this, &Material::onGlobalParamChanged);
        }
        m_BaseMaterial->onParamChanged.bind(this, &Material::onBaseMaterialParamChanged);
        return true;
    }

    void Material::onObjectDescriptorDestroying()
    {
        clearMaterial();

        Super::onObjectDescriptorDestroying();
    }
    void Material::clearMaterial()
    {
        if (m_Material != nullptr)
        {
            Engine* engine = getEngine();

            engine->getSubsystem<AssetsEngineSubsystem>()->onEngineInternalParamChanged.unbind(this, &Material::onGlobalParamChanged);
            if (m_BaseMaterial != nullptr)
            {
                m_BaseMaterial->onParamChanged.unbind(this, &Material::onBaseMaterialParamChanged);
            }
            for (const auto& texture : m_ReferencedTextures)
            {
	            if (texture.value.updatePtr() != nullptr)
	            {
		            texture.value->onDestroying.unbind(this, &Material::onTextureDestroying);
	            }
            }
            
            if (m_Material != nullptr)
            {
                engine->getRenderEngine()->destroyMaterial(m_Material);
            }

            onParamChanged.clear();
            m_OverridedParams.clear();
            m_ReferencedTextures.clear();
            m_Material = nullptr;
            m_BaseShader = nullptr;
            m_BaseMaterial = nullptr;
        }
    }

    void Material::onBaseMaterialParamChanged(const jstringID& paramName)
    {
        if (!m_OverridedParams.contains(paramName))
        {
            if (CopyMaterialParam(m_Material, m_BaseMaterial->getMaterial()->getMaterialParams(), paramName))
            {
	            onParamChanged.call(paramName);
            }
        }
    }

    bool Material::isGlobalMaterialParam(const jstringID& name) const
    {
        const EngineObjectPtr<Shader> shader = getBaseShader();
        return (shader != nullptr) && shader->getGlobalParamNames().contains(name);
    }
    void Material::onGlobalParamChanged(AssetsEngineSubsystem* subsystem, const jstringID& globalParamName)
    {
        for (const auto& shaderGlobalParam : getBaseShader()->getGlobalParamNames())
        {
            if (shaderGlobalParam.value == globalParamName)
            {
                CopyMaterialParam(m_Material, shaderGlobalParam.key, subsystem->getGlobalParams(), globalParamName);
				break;
            }
        }
    }
    
    template<>
    bool Material::updateParamValue<MaterialParamType::Texture>(const jstringID& name, const EngineObjectPtr<TextureBase>& value)
    {
        const EngineObjectPtr<TextureBase>* prevTexture = m_ReferencedTextures.find(name);
        if ((prevTexture != nullptr) && (prevTexture->updatePtr() != nullptr))
        {
	        (*prevTexture)->onDestroying.unbind(this, &Material::onTextureDestroying);
        }
        
        JumaRE::TextureBase* textureBase = value.updatePtr() != nullptr ? value->getTextureBase() : nullptr;
        if (textureBase == nullptr)
        {
	        m_ReferencedTextures.remove(name);
        }
        else
        {
            value->onDestroying.bind(this, &Material::onTextureDestroying);
	        m_ReferencedTextures.add(name, value);
        }
        return m_Material->setParamValue<JumaRE::ShaderUniformType::Texture>(name, textureBase);
    }
    bool Material::resetParamValue(const jstringID& name)
    {
        if (isGlobalMaterialParam(name))
        {
	        return false;
        }
        
        const EngineObjectPtr<TextureBase>* prevTexture = m_ReferencedTextures.find(name);
        if ((prevTexture != nullptr) && (prevTexture->updatePtr() != nullptr))
        {
	        (*prevTexture)->onDestroying.unbind(this, &Material::onTextureDestroying);
        }
        m_ReferencedTextures.remove(name);
        m_OverridedParams.remove(name);

        bool valueUpdated;
        if (m_BaseMaterial != nullptr)
        {
	        valueUpdated = CopyMaterialParam(m_Material, m_BaseMaterial->getMaterial()->getMaterialParams(), name);
        }
        else
        {
	        valueUpdated = m_Material->resetParamValue(name);
        }
        if (!valueUpdated)
        {
            return false;
        }

        onParamChanged.call(name);
        return true;
    }
    void Material::onTextureDestroying(EngineContextObject* object)
    {
        jstringID name = jstringID_NONE;
        for (const auto& texture : m_ReferencedTextures)
        {
	        if (texture.value.get() == object)
	        {
		        name = texture.key;
                break;
	        }
        }
        setParamValue<MaterialParamType::Texture>(name, nullptr);
    }
}
