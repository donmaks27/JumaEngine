// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_USE_GRAPHIC_API_OPENGL)
#   if defined(JUMAENGINE_USE_WINDOW_LIB_GLFW)

#include "OpenGL/RenderManager_OpenGL_GLFW.h"

namespace JumaEngine
{
    typedef RenderManager_OpenGL_GLFW RenderManager_OpenGL;
}

#   endif
#endif
