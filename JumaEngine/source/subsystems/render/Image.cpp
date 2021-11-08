﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Image.h"
#include "utils/jlog.h"

namespace JumaEngine
{
    bool Image::init(const glm::uvec2& size, const ImageFormat format, const uint8* data)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Image already initialized"));
            return false;
        }
        if ((size.x == 0) || (size.y == 0) || (format == ImageFormat::None) || (data == nullptr))
        {
            JUMA_LOG(warning, JSTR("Wrong input data"));
            return false;
        }
        if (!initInternal(size, format, data))
        {
            return false;
        }

        markAsInitialized(size, format);
        return true;
    }

    void Image::clear()
    {
        if (isValid())
        {
            clearInternal();
            m_Size = glm::uvec2(0);
            m_Format = ImageFormat::None;
            m_Initialized = false;
        }
    }

    uint32 Image::getPixelSize(const ImageFormat format)
    {
        switch (format)
        {
        case ImageFormat::R8G8B8A8:
        case ImageFormat::B8G8R8A8:
        case ImageFormat::DEPTH_SFLOAT32:
        case ImageFormat::DEPTH_UNORM24_STENCIL_UINT8:
            return 4;

        case ImageFormat::DEPTH_SFLOAT32_STENCIL_UINT8: 
            return 5;

        default: ;
        }
        return 0;
    }
}
