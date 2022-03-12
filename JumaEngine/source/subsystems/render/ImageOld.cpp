// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "ImageOld.h"
#include "jutils/jlog.h"

namespace JumaEngine
{
    bool ImageOld::init(const math::uvector2& size, const TextureFormat format, const uint8* data)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("ImageOld already initialized"));
            return false;
        }
        if ((size.x == 0) || (size.y == 0) || (format == TextureFormat::None) || (data == nullptr))
        {
            JUMA_LOG(warning, JSTR("Wrong input data"));
            return false;
        }
        if (!initInternal(size, format, data))
        {
            return false;
        }

        m_Size = size;
        m_Format = format;
        markAsInitialized();
        return true;
    }

    void ImageOld::clear()
    {
        if (isValid())
        {
            clearInternal();
            m_Size = math::uvector2(0);
            m_Format = TextureFormat::None;
            m_Initialized = false;
        }
    }

    uint32 ImageOld::getPixelSize(const TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::RGBA:
        case TextureFormat::BGRA:
        case TextureFormat::SFLOAT32:
        case TextureFormat::UNORM24_UINT8:
            return 4;

        case TextureFormat::SFLOAT32_UINT8: 
            return 5;

        default: ;
        }
        return 0;
    }
}
