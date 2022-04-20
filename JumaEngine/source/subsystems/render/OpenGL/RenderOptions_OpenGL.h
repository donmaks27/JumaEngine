// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/RenderOptions.h"

#include "subsystems/window/WindowID.h"

namespace JumaEngine
{
    struct RenderOptions_OpenGL : RenderOptions
    {
        window_id_type windowID = INVALID_WINDOW_ID;
    };
}

#endif