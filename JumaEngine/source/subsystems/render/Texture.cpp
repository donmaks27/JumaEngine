// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Texture.h"

#include "RenderSubsystem.h"
#include "engine/Engine.h"
#include "texture/TextureData.h"

namespace JumaEngine
{
    Texture::~Texture()
    {
        clearData();
    }

    bool Texture::init(const TextureData* data)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Texture already initialized"));
            return false;
        }
        if ((data == nullptr) || !data->isValid())
        {
            JUMA_LOG(warning, JSTR("Invalid texture data"));
            return false;
        }

        m_Data = data;
        markAsInitialized();
        return true;
    }

    Texture_RenderAPIObject* Texture::createRenderAPIObjectInternal()
    {
        return getOwnerEngine()->getRenderSubsystem()->createTextureObject();
    }

    void Texture::clearData()
    {
        clearRenderAPIObject();
        if (m_Data != nullptr)
        {
            delete m_Data;
            m_Data = nullptr;
        }
    }
}
