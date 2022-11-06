// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../../../include/JumaEngine/subsystems/textures/TexturesSubsystem.h"

#include "../../../include/JumaEngine/Engine.h"

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

    Texture* TexturesSubsystem::getTexture(const jstringID& textureName)
    {
        Texture* texturePtr = m_Textures.find(textureName);
        if (texturePtr != nullptr)
        {
            return texturePtr;
        }

        Texture* texture = getEngine()->registerObject(&m_Textures.add(textureName));
        if (!texture->loadTexture(textureName))
        {
            JUTILS_LOG(error, JSTR("Failed to load texture {}"), textureName.toString());
            return nullptr;
        }

        JUTILS_LOG(correct, JSTR("Loaded texture {}"), textureName.toString());
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
