// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "jutils/jstringID.h"

namespace JumaEngine
{
    class RenderPipeline;

    struct RenderOptions
    {
        RenderPipeline* renderPipeline = nullptr;
        jstringID renderTargetName = jstringID_NONE;
    };
}