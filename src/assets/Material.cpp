// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/Material.h"

#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
    bool Material::getUniformNameForParam(const jstringID& paramName, jstringID& outUniformName) const
    {
        const jstringID* uniformName = m_MaterialParamsUniform.find(paramName);
        if (uniformName == nullptr)
        {
            return false;
        }
        outUniformName = *uniformName;
        return true;
    }

	template<>
	bool Material::updateParamValue<JumaRenderEngine::ShaderUniformType::Texture>(const jstringID& paramName, 
        const jstringID& uniformName, const EngineObjectPtr<TextureBase>& value)
	{
		const EngineObjectPtr<TextureBase>* prevTexture = m_ReferencedTextures.find(paramName);
        if ((prevTexture != nullptr) && (prevTexture->get() != nullptr))
        {
	        (*prevTexture)->onDestroying.unbind(this, &Material::onTextureDestroying);
        }
        
        JumaRE::TextureBase* textureBase = value != nullptr ? value->getTextureBase() : nullptr;
        if (textureBase == nullptr)
        {
	        m_ReferencedTextures.remove(paramName);
        }
        else
        {
            value->onDestroying.bind(this, &Material::onTextureDestroying);
	        m_ReferencedTextures.add(paramName, value);
        }
        return m_Material->setParamValue<JumaRE::ShaderUniformType::Texture>(uniformName, textureBase);
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

    bool Material::resetParamValue(const jstringID& paramName)
	{
        if (m_Material == nullptr)
        {
	        return false;
        }
        jstringID uniformName;
        if (!getUniformNameForParam(paramName, uniformName))
        {
            return false;
        }
        if (!resetParamValueInternal(paramName, uniformName))
        {
	        return false;
        }
        onParamChanged.call(paramName, uniformName);
        return true;
	}
    bool Material::resetParamValueInternal(const jstringID& paramName, const jstringID& uniformName)
    {
        const EngineObjectPtr<TextureBase>* prevTexture = m_ReferencedTextures.find(paramName);
        if ((prevTexture != nullptr) && (prevTexture->get() != nullptr))
        {
	        (*prevTexture)->onDestroying.unbind(this, &Material::onTextureDestroying);
        }
        m_ReferencedTextures.remove(paramName);
        return true;
    }
    
    bool Material::getTextureParamValue(const jstringID& name, EngineObjectPtr<TextureBase>& outValue) const
	{
        const EngineObjectPtr<TextureBase>* valuePtr = m_ReferencedTextures.find(name);
        if (valuePtr == nullptr)
        {
	        return false;
        }
        outValue = *valuePtr;
        return true;
	}

    void Material::clearAsset()
    {
        if (m_Material != nullptr)
        {
			Super::clearAsset();
            clearMaterial();
        }
    }
    void Material::clearMaterial()
    {
        for (const auto& texture : m_ReferencedTextures)
        {
            if (texture.value.get() != nullptr)
            {
	            texture.value->onDestroying.unbind(this, &Material::onTextureDestroying);
            }
        }
        
        if (m_Material != nullptr)
        {
            getEngine()->getRenderEngine()->destroyMaterial(m_Material);
        }

        onParamChanged.clear();
        m_ReferencedTextures.clear();
        m_MaterialParamsUniform.clear();
        m_MaterialParamsOrder.clear();
        m_Material = nullptr;
    }
}
