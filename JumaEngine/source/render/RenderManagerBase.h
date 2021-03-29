// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include "RenderAPI.h"

namespace JumaEngine
{
    class ShaderBase;
    class VertexBufferBase;

    class RenderManagerBase : public EngineContextObject
    {
    public:
        RenderManagerBase() = default;
        virtual ~RenderManagerBase() override = default;

		virtual RenderAPI getRenderAPI() const { return RenderAPI::None; }
    	
        virtual bool init() = 0;
        virtual void terminate() = 0;

        virtual void onRenderStart() = 0;

        virtual ShaderBase* createShader() = 0;
        virtual VertexBufferBase* createVertextBuffer() = 0;
    };
}
