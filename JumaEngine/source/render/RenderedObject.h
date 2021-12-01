// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "render/window/window_id.h"

namespace JumaEngine
{
    struct RenderParams
    {
        bool invertFacesOrientation = false;
    };

    class IRenderedObject
    {
    protected:
        IRenderedObject() = default;
        virtual ~IRenderedObject() = default;

    public:

        virtual void render(window_id windowID, const RenderParams& renderParams) {}
    };
}
