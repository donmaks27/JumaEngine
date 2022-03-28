﻿// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    enum class RenderPresentMode : uint8
    {
        IMMEDIATE,
        VSYNC,
        TRIPLE_BUFFER
    };
}