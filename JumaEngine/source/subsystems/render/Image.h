// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"

#include "utils/math/vector2.h"
#include "ImageFormat.h"

namespace JumaEngine
{
    class Image : public EngineContextObject
    {
        JUMAENGINE_ABSTRACT_CLASS(Image, EngineContextObject)

    public:
        Image() = default;
        virtual ~Image() override = default;

        bool isValid() const { return m_Initialized; }
        bool init(const math::uvector2& size, ImageFormat format, const uint8* data);
        void clear();

        const math::uvector2& getSize() const { return m_Size; }
        ImageFormat getFormat() const { return m_Format; }

        static uint32 getPixelSize(ImageFormat format);

    protected:

        void markAsInitialized() { m_Initialized = true; }
        void markAsInitialized(const math::uvector2& size, const ImageFormat format)
        {
            m_Size = size;
            m_Format = format;
            markAsInitialized();
        }

        virtual bool initInternal(const math::uvector2& size, ImageFormat format, const uint8* data) = 0;

        virtual void clearInternal() = 0;

    private:

        bool m_Initialized = false;

        math::uvector2 m_Size = math::uvector2(0);
        ImageFormat m_Format = ImageFormat::None;
    };
}
