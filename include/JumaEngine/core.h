// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include <jutils/jlog.h>

#ifdef JUMAENGINE_ENABLED_EDITORENGINE
    #undef JUMAENGINE_ENABLED_GAMEENGINE
#else
    #define JUMAENGINE_ENABLED_GAMEENGINE
#endif

namespace JumaEngine
{
    using namespace jutils;
}
namespace JE = JumaEngine;
