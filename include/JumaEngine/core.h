// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include <jutils/log.h>

#ifdef JUMAENGINE_MODE_EDITOR
    #undef JUMAENGINE_MODE_GAME
#else
    #define JUMAENGINE_MODE_GAME
#endif

namespace JumaEngine
{
    using namespace jutils;
}
namespace JE = JumaEngine;
