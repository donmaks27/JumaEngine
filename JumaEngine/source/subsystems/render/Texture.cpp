// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "Texture.h"

#include "RenderSubsystem.h"
#include "engine/Engine.h"

namespace JumaEngine
{
    Texture::~Texture()
    {
        if (isValid())
        {
            onClear.call(this);
        }
        clearData();
    }

    bool Texture::init(const TextureFormat format, const math::uvector2& size, const uint8* data)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Texture already initialized"));
            return false;
        }
        if ((data == nullptr) || (format == TextureFormat::None) || (data == nullptr))
        {
            JUMA_LOG(error, JSTR("Invalid input params"));
            return false;
        }

        m_Size = size;
        m_Format = format;
        m_Data = data;
        markAsInitialized();
        return true;
    }

    Texture_RenderAPIObject* Texture::createRenderAPIObjectInternal()
    {
        return getOwnerEngine()->getRenderSubsystem()->createTextureObject();
    }

    void Texture::clearInternal()
    {
        onClear.call(this);
        onClear.clear();

        clearData();
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
