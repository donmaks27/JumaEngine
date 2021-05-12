// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "window/window_id.h"

namespace JumaEngine
{
    struct RenderParams
    {
        bool invertFacesOrientation = false;
    };

    class IRenderInterface
    {
    protected:
        IRenderInterface() = default;
        virtual ~IRenderInterface() = default;

    public:

        virtual void render(window_id windowID, const RenderParams& renderParams) {}
    };
}
