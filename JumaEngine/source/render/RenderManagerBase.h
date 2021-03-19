// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"

namespace JumaEngine
{
    class VertexBufferBase;

    class RenderManagerBase : public EngineContextObject
    {
    public:
        RenderManagerBase() = default;
        virtual ~RenderManagerBase() override = default;

        virtual bool init() = 0;

        virtual void startFrameRender() = 0;

        virtual void terminate() {}

        virtual VertexBufferBase* createVertextBufferRender() = 0;
    };
}
