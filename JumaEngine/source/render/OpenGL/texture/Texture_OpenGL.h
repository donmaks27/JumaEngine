// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "render/texture/TextureBase.h"

namespace JumaEngine
{
    class Texture_OpenGL : public TextureBase
    {
        JUMAENGINE_CLASS(Texture_OpenGL, TextureBase)

    public:
        Texture_OpenGL() = default;
        virtual ~Texture_OpenGL() override;
        
        virtual void activate(uint32 index) override;
        virtual void deactivate(uint32 index) override;

    protected:

        virtual bool initInternal(const uint8* data) override;

    private:

        uint32 m_TextureIndex = 0;


        void terminate_OpenGL();
    };
}

#endif
