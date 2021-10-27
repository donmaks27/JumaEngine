// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"
#include "ImageFormat.h"

#include <glm/vec2.hpp>

namespace JumaEngine
{
    class Image : public EngineContextObject
    {
        JUMAENGINE_ABSTRACT_CLASS(Image, EngineContextObject)

    public:
        Image() = default;
        virtual ~Image() override = default;

        bool isValid() const { return m_Initialized; }
        bool init(const glm::uvec2& size, ImageFormat format, const uint8* data);
        void clear();

    protected:

        void markAsInitialized() { m_Initialized = true; }

        virtual bool initInternal(const glm::uvec2& size, ImageFormat format, const uint8* data) = 0;

        virtual void clearInternal() = 0;

    private:

        bool m_Initialized = false;

        glm::uvec2 m_Size = glm::uvec2(0);
        ImageFormat m_Format = ImageFormat::None;
    };
}
