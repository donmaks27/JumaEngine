// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/Image.h"

namespace JumaEngine
{
    class Image_OpenGL final : public Image
    {
        JUMAENGINE_CLASS(Image_OpenGL, Image)

    public:
        Image_OpenGL() = default;
        virtual ~Image_OpenGL() override;

        void activate(uint32 index) const;
        static void deactivate(uint32 index);

    protected:

        virtual bool initInternal(const math::uvector2& size, ImageFormat format, const uint8* data) override;
        virtual void clearInternal() override;

    private:

        uint32 m_Index = 0;


        static uint32 imageFormatToGLFormat(ImageFormat format);

        void clearOpenGL();
    };
}

#endif
