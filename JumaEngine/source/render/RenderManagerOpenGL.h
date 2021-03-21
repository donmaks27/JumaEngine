﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderManagerBase.h"

namespace JumaEngine
{
    class ShaderBase;
	class Camera;

    class RenderManagerOpenGL : public RenderManagerBase
    {
    public:
        RenderManagerOpenGL() = default;
        virtual ~RenderManagerOpenGL() override = default;

        virtual bool init() override;

        virtual void render() override;

        virtual void terminate() override;

        virtual VertexBufferBase* createVertextBufferRender() override;

    private:

        ShaderBase* m_Shader = nullptr;
        VertexBufferBase* m_VertexBuffer = nullptr;
    	Camera* m_Camera = nullptr;
    };
}
