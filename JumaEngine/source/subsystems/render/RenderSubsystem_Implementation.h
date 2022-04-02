// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "RenderSubsystemInfo.h"
#include "Vulkan/RenderSubsystemInfo_Vulkan.h"

namespace JumaEngine
{
    inline RenderSubsystem_RenderAPIObject* createRenderSubsystemRenderAPIObject(const RenderAPI renderAPI)
    {
        switch (renderAPI)
        {
        case RenderAPI::Vulkan: return RenderSubsystemInfo<RenderAPI::Vulkan>::createRenderAPIObject();
        default: ;
        }
        JUMA_LOG(error, JSTR("Unsupported render API"));
        return nullptr;
    }
}
