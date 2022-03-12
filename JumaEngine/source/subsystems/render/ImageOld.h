// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"

#include "jutils/math/vector2.h"
#include "texture/TextureFormat.h"

namespace JumaEngine
{
    class ImageOld : public EngineContextObject
    {
        JUMAENGINE_ABSTRACT_CLASS(ImageOld, EngineContextObject)

    public:
        ImageOld() = default;
        virtual ~ImageOld() override = default;

        bool isValid() const { return m_Initialized; }
        bool init(const math::uvector2& size, TextureFormat format, const uint8* data);
        void clear();

        const math::uvector2& getSize() const { return m_Size; }
        TextureFormat getFormat() const { return m_Format; }

        static uint32 getPixelSize(TextureFormat format);

    protected:

        math::uvector2 m_Size = math::uvector2(0);
        TextureFormat m_Format = TextureFormat::None;


        void markAsInitialized() { m_Initialized = true; }

        virtual bool initInternal(const math::uvector2& size, TextureFormat format, const uint8* data) = 0;

        virtual void clearInternal() = 0;

    private:

        bool m_Initialized = false;
    };
}
