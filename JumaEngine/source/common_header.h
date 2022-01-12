// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#if defined(_DEBUG) || !defined(NDEBUG)
    #define JDEBUG 1
#else
    #define JDEBUG 0
#endif

/*#if JDEBUG && _WIN32

#include <crtdbg.h>
    #define debug_new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new debug_new

#endif*/



#include "enabled_engine_parts.h"

#include "jutils/type_defines.h"

#if !JDEBUG
    #define JUTILS_LOG_DISABLED
#endif
#include "jutils/jlog.h"
#define JUMA_LOG(type, message) JUTILS_LOG_WRITE(type, message)
#define JUMA_LOG_CUSTOM(type, message) JUTILS_LOG_WRITE_CUSTOM(type, message)
#define JUMA_LOG_EMPTY(type, message) JUTILS_LOG_WRITE_EMPTY(type, message)

namespace JumaEngine
{
    using namespace jutils;
}
namespace JE = JumaEngine;
