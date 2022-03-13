// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/Texture.h"

namespace JumaEngine
{
    TextureFormat GetTextureFormatByOpenGLFormat(uint32 format);
    uint32 GetOpenGLFormatByTextureFormat(TextureFormat format);

    class TextureObject_OpenGL : public TextureObject
    {
    public:
        TextureObject_OpenGL() = default;
        virtual ~TextureObject_OpenGL() override;

        bool bind(uint32 index) const;
        static void unbind(uint32 index);

    protected:

        virtual bool initInternal() override;
        virtual void clearInternal() override { clearOpenGL(); }

    private:

        uint32 m_ImageIndex = 0;


        void clearOpenGL();
    };
}

#endif
