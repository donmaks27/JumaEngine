// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/textures/TexturesSubsystem.h"

#include "JumaEngine/Engine.h"

namespace JumaEngine
{
    bool TexturesSubsystem::initSubsystem()
    {
        if (!Super::initSubsystem())
        {
            return false;
        }

        getEngine()->getRenderEngine()->onDestroying.bind(this, &TexturesSubsystem::onRenderEngineDestroying);
        return true;
    }
    void TexturesSubsystem::clearSubsystem()
    {
        clear();

        Super::clearSubsystem();
    }

    Texture* TexturesSubsystem::getEngineTexture(const jstringID& textureName)
    {
        static jstringID contentFolder = JSTR("content_engine");
        return getTexture(m_EngineTextures, textureName, contentFolder);
    }
    Texture* TexturesSubsystem::getTexture(const jstringID& textureName)
    {
        static jstringID contentFolder = JSTR("content");
        return getTexture(m_Textures, textureName, contentFolder);
    }
    Texture* TexturesSubsystem::getTexture(jmap<jstringID, Texture>& texturesList, const jstringID& textureName, const jstringID& contentFolder) const
    {
        Texture* texturePtr = texturesList.find(textureName);
        if (texturePtr != nullptr)
        {
            return texturePtr;
        }

        Texture* texture = getEngine()->registerObject(&texturesList.add(textureName));
        if (!texture->loadTexture(textureName, contentFolder))
        {
            JUTILS_LOG(error, JSTR("Failed to load texture {} from {}"), textureName.toString(), contentFolder.toString());
            return nullptr;
        }

        JUTILS_LOG(correct, JSTR("Loaded texture {} from {}"), textureName.toString(), contentFolder.toString());
        return texture;
    }

    void TexturesSubsystem::clear()
    {
        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        if (renderEngine != nullptr)
        {
            renderEngine->onDestroying.unbind(this, &TexturesSubsystem::onRenderEngineDestroying);
        }

        for (auto& texture : m_Textures)
        {
            texture.value.clearTexture();
        }
        m_Textures.clear();
    }
}
