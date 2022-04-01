// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "Vulkan/WindowSubsystemInfo_Vulkan.h"

namespace JumaEngine
{
    template<RenderAPI API>
    WindowSubsystemRenderAPIObject* createWindowSubsystemRenderAPIObject()
    {
        return WindowSubsystemInfo<API>::createRenderAPIObject();
    }
    inline WindowSubsystemRenderAPIObject* createWindowSubsystemRenderAPIObject(const RenderAPI renderAPI)
    {
        switch (renderAPI)
        {
        case RenderAPI::Vulkan: return createWindowSubsystemRenderAPIObject<RenderAPI::Vulkan>();
        default: ;
        }
        JUMA_LOG(error, JSTR("Unsupported render API"));
        return nullptr;
    }
}
