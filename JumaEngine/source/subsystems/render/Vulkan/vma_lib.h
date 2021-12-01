// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

#ifdef debug_new

#undef new
#include <vma/vk_mem_alloc.h>
#define new debug_new

#else

#include <vma/vk_mem_alloc.h>

#endif

#endif
