// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    struct RenderQuery {};

    struct RenderOptions
    {
        RenderQuery query;

        bool invertFacesOrientation = false;
    };
}
