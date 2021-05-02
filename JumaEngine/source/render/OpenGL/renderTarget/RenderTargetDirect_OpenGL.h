// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)

#include "render/renderTarget/RenderTargetDirectBase.h"

namespace JumaEngine
{
    class RenderTargetDirect_OpenGL : public RenderTargetDirectBase
    {
    public:
        RenderTargetDirect_OpenGL() = default;
        virtual ~RenderTargetDirect_OpenGL() override = default;

    protected:

        virtual void clearFrame() override;
    };
}

#endif
