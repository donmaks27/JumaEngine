// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "TextureBase.h"

namespace JumaEngine
{
    bool TextureBase::init(const glm::uvec2& size, const TextureFormat format, const uint8* data)
    {
        if (!isInit())
        {
            m_Size = size;
            m_Format = format;
            if ((m_Size.x > 0) && (m_Size.y > 0) && (m_Format != TextureFormat::None) && (data != nullptr))
            {
                if (initInternal(data))
                {
                    m_Initialized = true;
                    return true;
                }
            }
        }
        return false;
    }
}
