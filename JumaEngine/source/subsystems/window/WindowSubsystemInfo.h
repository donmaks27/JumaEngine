// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "subsystems/render/RenderAPI.h"
#include "WindowSubsystem.h"

namespace JumaEngine
{
    template<RenderAPI API>
    struct WindowSubsystemInfo : std::false_type
    {
        static WindowSubsystemRenderAPIObject* createRenderAPIObject()
        {
            JUMA_LOG(error, JSTR("Unsupported render API"));
            return nullptr;
        }
    };
}
