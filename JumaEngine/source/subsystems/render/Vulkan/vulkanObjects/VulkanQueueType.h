// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_VULKAN)

namespace JumaEngine
{
    enum class VulkanQueueType
    {
        Graphics,
        Present,
        Transfer
    };
}

#endif
