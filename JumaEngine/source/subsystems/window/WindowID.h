// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "jutils/juid.h"

namespace JumaEngine
{
    using window_id_type = uint32;
    constexpr window_id_type INVALID_WINDOW_ID = juid<window_id_type>::invalidUID;
}
