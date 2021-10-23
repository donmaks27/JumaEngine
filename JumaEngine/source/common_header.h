// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#define GLM_FORCE_INLINE
#define GLM_FORCE_XYZW_ONLY

#if defined(_DEBUG) || !defined(NDEBUG)
#define JDEBUG 1
#endif

#if JDEBUG && _WIN32

#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

#endif


/**
 * Possible macros:
 *
 * JUMAENGINE_INCLUDE_RENDER_API_OPENGL - can use OpenGL render API
 *
 * JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW - can use GLFW window lib
 *
 * JUMAENGINE_SINGLE_WINDOW - should create only one window
 */

#define JUMAENGINE_INCLUDE_RENDER_API_OPENGL
#define JUMAENGINE_INCLUDE_WINDOW_LIB_GLFW

#define JUMAENGINE_SINGLE_WINDOW


#include "utils/int_defines.h"
#include "utils/jstring.h"

namespace JumaEngine
{
    using namespace jutils;
}
namespace JE = JumaEngine;
