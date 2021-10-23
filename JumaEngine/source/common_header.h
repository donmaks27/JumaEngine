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
 * JUMAENGINE_USE_GRAPHIC_API_OPENGL - can use OpenGL graphic API
 *
 * JUMAENGINE_USE_WINDOW_LIB_GLFW - can use GLFW window lib
 *
 * JUMAENGINE_SINGLE_WINDOW - should create only one window
 */

#define JUMAENGINE_USE_GRAPHIC_API_OPENGL
#define JUMAENGINE_USE_WINDOW_LIB_GLFW

#define JUMAENGINE_SINGLE_WINDOW


#include <cstdint>

namespace JumaEngine { }
namespace JE = JumaEngine;

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

#include "utils/jstring.h"
#define JTEXT(str) u8 ## str
#define TO_JTEXT(value) std::to_string(value)

#define JUMAENGINE_CONCAT_HELPER(...) __VA_ARGS__
