// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/Material.h"

#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
	template<>
	bool Material::updateParamValue<JumaRenderEngine::ShaderUniformType::Texture>(const jstringID& name, 
		const EngineObjectPtr<TextureBase>& value)
	{
		const EngineObjectPtr<TextureBase>* prevTexture = m_ReferencedTextures.find(name);
        if ((prevTexture != nullptr) && (prevTexture->get() != nullptr))
        {
	        (*prevTexture)->onDestroying.unbind(this, &Material::onTextureDestroying);
        }
        
        JumaRE::TextureBase* textureBase = value != nullptr ? value->getTextureBase() : nullptr;
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

    bool Material::resetParamValue(const jstringID& name)
	{
        if (m_Material == nullptr)
        {
	        return false;
        }

        const EngineObjectPtr<TextureBase>* prevTexture = m_ReferencedTextures.find(name);
        if ((prevTexture != nullptr) && (prevTexture->get() != nullptr))
        {
	        (*prevTexture)->onDestroying.unbind(this, &Material::onTextureDestroying);
        }
        m_ReferencedTextures.remove(name);

        if (!resetParamValueInternal(name))
        {
	        return false;
        }
        onParamChanged.call(name);
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
        m_Material = nullptr;
    }
}
