// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "TextureData.h"

namespace JumaEngine
{
    TextureData::~TextureData()
    {
        clear();
    }

    bool TextureData::init(const math::uvector2& size, const TextureFormat format, const uint8* data)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Texture data already initlaized"));
            return false;
        }

        m_Size = size;
        m_Format = format;
        m_Data = data;
        return true;
    }

    void TextureData::clear()
    {
        if (isValid())
        {
            delete m_Data;
            m_Data = nullptr;
            m_Size = { 0, 0 };
            m_Format = TextureFormat::None;
        }
    }
}
