// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"

namespace JumaEngine
{
    class ShaderBase;
    class VertexBufferBase;

    class RenderManagerBase : public EngineContextObject
    {
    public:
        RenderManagerBase() = default;
        virtual ~RenderManagerBase() override = default;

        virtual bool init() = 0;

        virtual void render() = 0;

        virtual void terminate() {}

        virtual ShaderBase* createShader() = 0;
        virtual VertexBufferBase* createVertextBuffer() = 0;
    };
}
