// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "RenderAPI.h"
#include "RenderSubsystem.h"

namespace JumaEngine
{
    template<RenderAPI API>
    struct RenderSubsystemInfo : std::false_type
    {
        static RenderSubsystem_RenderAPIObject* createRenderAPIObject()
        {
            JUMA_LOG(error, JSTR("Unsupported render API"));
            return nullptr;
        }
    };
}
