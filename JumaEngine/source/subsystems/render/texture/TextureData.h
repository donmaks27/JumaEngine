// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "jutils/math/vector2.h"
#include "TextureFormat.h"

namespace JumaEngine
{
    class TextureData final
    {
    public:
        TextureData() = default;
        ~TextureData();

        bool init(const math::uvector2& size, TextureFormat format, const uint8* data);
        bool isValid() const { return m_Data != nullptr; }
        void clear();

        const math::uvector2& getSize() const { return m_Size; }
        TextureFormat getFormat() const { return m_Format; }
        const uint8* getData() const { return m_Data; }

    private:

        math::uvector2 m_Size = { 0, 0 };
        TextureFormat m_Format = TextureFormat::None;
        const uint8* m_Data = nullptr;
    };
}
