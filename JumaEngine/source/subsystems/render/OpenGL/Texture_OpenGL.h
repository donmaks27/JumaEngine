// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/Texture.h"

#include "subsystems/render/texture/TextureFormat.h"

namespace JumaEngine
{
    TextureFormat GetTextureFormatByOpenGLFormat(uint32 format);
    uint32 GetOpenGLFormatByTextureFormat(TextureFormat format);

    class Texture_RenderAPIObject_OpenGL : public Texture_RenderAPIObject
    {
    public:
        Texture_RenderAPIObject_OpenGL() = default;
        virtual ~Texture_RenderAPIObject_OpenGL() override;

        bool bind(const uint32 index) const { return bind(m_ImageIndex, index); }
        static bool bind(uint32 imageIndex, uint32 index);
        static void unbind(uint32 index);

    protected:

        virtual bool initInternal() override;

    private:

        uint32 m_ImageIndex = 0;


        void clearOpenGL();
    };
}

#endif
