// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/RenderPipeline.h"

namespace JumaEngine
{
    class RenderPipeline_RenderAPIObject_OpenGL : public RenderPipeline_RenderAPIObject
    {
    public:
        RenderPipeline_RenderAPIObject_OpenGL() = default;
        virtual ~RenderPipeline_RenderAPIObject_OpenGL() override;

    protected:

        virtual bool initInternal() override;
        
        virtual bool renderPipeline() override;

    private:

        void clearData();
    };
}

#endif
