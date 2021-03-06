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


namespace JumaEngine { }
namespace JE = JumaEngine;

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long int64;
typedef unsigned long uint64;

#include "utils/jstring.h"
#define JTEXT(str) u8 ## str
#define TO_JTEXT(value) std::to_string(value)

#define JUMAENGINE_CONCAT_HELPER(...) __VA_ARGS__
