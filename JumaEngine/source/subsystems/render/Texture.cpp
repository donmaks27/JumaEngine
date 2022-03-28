// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Texture.h"

#include "RenderSubsystem.h"
#include "engine/Engine.h"
#include "texture/TextureData.h"

namespace JumaEngine
{
    Texture::~Texture()
    {
        clear();
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
        m_Initialized = true;
        return true;
    }

    void Texture::clear()
    {
        clearRenderObject();
        if (m_Data != nullptr)
        {
            delete m_Data;
            m_Data = nullptr;
        }
    }

    bool Texture::createRenderObject()
    {
        if (!isValid())
        {
            JUMA_LOG(warning, JSTR("Shader not initialized"));
            return false;
        }

        RenderSubsystem* renderSubsystem = getOwnerEngine()->getRenderSubsystem();
        m_RenderObject = renderSubsystem->createTextureObject();
        if (!m_RenderObject->init(this))
        {
            delete m_RenderObject;
            m_RenderObject = nullptr;
            return false;
        }
        return true;
    }
    void Texture::clearRenderObject()
    {
        if (m_RenderObject != nullptr)
        {
            delete m_RenderObject;
            m_RenderObject = nullptr;
        }
    }
}
