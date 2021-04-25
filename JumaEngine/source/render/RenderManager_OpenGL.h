// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderManager.h"

namespace JumaEngine
{
    class RenderManager_OpenGL : public RenderManager
    {
    public:
        RenderManager_OpenGL() = default;
        virtual ~RenderManager_OpenGL() override = default;

        virtual RenderAPI getRenderAPI() const override { return RenderAPI::OpenGL; }
        
        ShaderBase* createShader() override;
        VertexBufferBase* createVertextBuffer() override;

        virtual void startRender() override;

    protected:

        virtual bool initInternal() override;

    private:

        typedef RenderManager Super;
    };
}
