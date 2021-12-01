// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "RenderOptions.h"

namespace JumaEngine
{
    class IRenderInterface
    {
    protected:
        IRenderInterface() = default;
        virtual ~IRenderInterface() = default;

    public:

        virtual void render(const RenderOptions& options) {}
    };
}
