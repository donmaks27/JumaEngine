// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    class RenderManagerBase
    {
    public:
        RenderManagerBase();
        virtual ~RenderManagerBase();

        virtual bool init() = 0;

        virtual void startFrameRender() = 0;

        virtual void terminate() {}
    };
}
