// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "render/RenderManagerBase.h"

namespace JumaEngine
{
    class RenderManagerBase_OpenGL : public RenderManagerBase
    {
    public:
        RenderManagerBase_OpenGL() = default;
        virtual ~RenderManagerBase_OpenGL() override = default;

        ShaderBase* createShader() override;
        VertexBufferBase* createVertextBuffer() override;

        virtual void startRender() override;

    protected:

        virtual bool initInternal() override;

    private:

        typedef RenderManagerBase Super;
    };
}

#endif
