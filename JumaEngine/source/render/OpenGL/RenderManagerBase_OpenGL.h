// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "render/RenderManagerBase.h"

namespace JumaEngine
{
    class RenderManagerBase_OpenGL : public RenderManagerBase
    {
        JUMAENGINE_CLASS(RenderManagerBase_OpenGL, RenderManagerBase)

    public:
        RenderManagerBase_OpenGL() = default;
        virtual ~RenderManagerBase_OpenGL() override = default;

    protected:

        virtual bool initInternal() override;

        virtual ShaderBase* createShaderInternal() override;
        virtual VertexBufferBase* createVertextBufferInternal() override;
        virtual TextureBase* createTextureInternal() override;
        virtual RenderTargetDirectBase* createRenderTargetDirectInternal() override;
    };
}

#endif
