// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderManagerBase.h"

namespace JumaEngine
{
    class RenderManagerOpenGL : public RenderManagerBase
    {
    public:
        RenderManagerOpenGL();
        virtual ~RenderManagerOpenGL() override;

        virtual bool init() override;

        virtual void startFrameRender() override;
    };
}
