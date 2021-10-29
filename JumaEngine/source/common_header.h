// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#define GLM_FORCE_INLINE
#define GLM_FORCE_XYZW_ONLY

#if defined(_DEBUG) || !defined(NDEBUG)
#define JDEBUG 1
#else
#define JDEBUG 0
#endif

#if JDEBUG && _WIN32

#include <crtdbg.h>
#define debug_new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new debug_new

#endif

#include "enabled_engine_parts.h"
#define JUMAENGINE_SINGLE_WINDOW


#include "utils/int_defines.h"
#include "utils/jstring.h"

namespace JumaEngine
{
    using namespace jutils;
}
namespace JE = JumaEngine;
