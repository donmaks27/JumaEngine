// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "jutils/jstringID.h"

namespace JumaEngine
{
    class RenderPipeline;
    class RenderTarget;

    struct RenderOptions
    {
        const RenderPipeline* renderPipeline = nullptr;
        jstringID renderTargetName = jstringID_NONE;
        const RenderTarget* renderTarget = nullptr;
    };
}