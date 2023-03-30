// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/Material.h"

#include "JumaEngine/assets/AssetsEngineSubsystem.h"
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

    bool Material::setAssetForTextureParamValue(const jstringID& paramName, const jstringID& uniformName, const jstringID& assetID)
    {
        const TextureReference* reference = m_ReferencedTextures.find(paramName);
        if ((reference != nullptr) && (reference->assetID == assetID))
        {
            return true;
        }
        const JumaRE::ShaderUniform* uniform = getShader()->getUniforms().find(uniformName);
        if ((uniform == nullptr) || (uniform->type != JumaRE::ShaderUniformType::Texture))
        {
            return false;
        }
        updateParamValue<MaterialParamType::Texture>(paramName, uniformName, nullptr);

        m_ReferencedTextures.add(paramName, { nullptr, assetID });
        return getEngine()->getSubsystem<AssetsEngineSubsystem>()->getAssetAsync(
            this, assetID, [this, paramName = paramName, uniformName = uniformName, assetID = assetID](const EngineObjectPtr<Asset>& asset)
            {
                const TextureReference* reference = m_ReferencedTextures.find(paramName);
                if ((reference == nullptr) || (reference->assetID != assetID))
                {
                    return;
                }
                const EngineObjectPtr<TextureBase> texture = asset.cast<TextureBase>();
                if (texture == nullptr)
                {
                    m_ReferencedTextures.remove(paramName);
                    return;
                }
                updateParamValue<MaterialParamType::Texture>(paramName, uniformName, texture);
            }
        );
    }

	template<>
	bool Material::updateParamValue<JumaRenderEngine::ShaderUniformType::Texture>(const jstringID& paramName, 
        const jstringID& uniformName, const EngineObjectPtr<TextureBase>& value)
	{
        const TextureReference* reference = m_ReferencedTextures.find(paramName);
        if ((reference != nullptr) && (reference->asset != nullptr))
        {
            reference->asset->onDestroying.unbind(this, &Material::onTextureDestroying);
        }

        JumaRE::TextureBase* textureBase = value != nullptr ? value->getTextureBase() : nullptr;
        if (textureBase == nullptr)
        {
            m_ReferencedTextures.remove(paramName);
        }
        else
        {
            value->onDestroying.bind(this, &Material::onTextureDestroying);
            m_ReferencedTextures.add(paramName, { value, value->getAssetID() });
        }
        return m_Material->setParamValue<JumaRE::ShaderUniformType::Texture>(uniformName, textureBase);
	}
	void Material::onTextureDestroying(EngineContextObject* object)
	{
        jstringID paramName = jstringID_NONE;
        for (const auto& reference : m_ReferencedTextures)
        {
            if (reference.value.asset.get() == object)
            {
                paramName = reference.key;
                break;
            }
        }
        setParamValue<MaterialParamType::Texture>(paramName, nullptr);
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
        const TextureReference* reference = m_ReferencedTextures.find(paramName);
        if ((reference != nullptr) && (reference->asset != nullptr))
        {
            reference->asset->onDestroying.unbind(this, &Material::onTextureDestroying);
        }
        m_ReferencedTextures.remove(paramName);
        return true;
    }
    
    bool Material::getTextureParamValue(const jstringID& paramName, EngineObjectPtr<TextureBase>& outValue) const
	{
        const TextureReference* reference = m_ReferencedTextures.find(paramName);
        if (reference == nullptr)
        {
            return false;
        }
        outValue = reference->asset;
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
        for (const auto& reference : m_ReferencedTextures)
        {
            if (reference.value.asset != nullptr)
            {
	            reference.value.asset->onDestroying.unbind(this, &Material::onTextureDestroying);
            }
        }
        
        if (m_Material != nullptr)
        {
            getEngine()->getRenderEngine()->destroyMaterial(m_Material);
        }

        onParamChanged.clear();
        m_ReferencedTextures.clear();
        m_MaterialParamsUniform.clear();
        m_MaterialParams.clear();
        m_Material = nullptr;
    }
}
