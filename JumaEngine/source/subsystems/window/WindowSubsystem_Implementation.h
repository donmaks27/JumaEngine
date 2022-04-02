// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "WindowSubsystemInfo.h"
#include "Vulkan/WindowSubsystemInfo_Vulkan.h"

namespace JumaEngine
{
    inline WindowSubsystem_RenderAPIObject* createWindowSubsystemRenderAPIObject(const RenderAPI renderAPI)
    {
        switch (renderAPI)
        {
        case RenderAPI::Vulkan: return WindowSubsystemInfo<RenderAPI::Vulkan>::createRenderAPIObject();
        default: ;
        }
        JUMA_LOG(error, JSTR("Unsupported render API"));
        return nullptr;
    }
}
