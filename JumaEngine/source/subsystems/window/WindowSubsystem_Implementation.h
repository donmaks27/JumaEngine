// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "WindowSubsystemInfo.h"
#include "Vulkan/WindowSubsystemInfo_Vulkan.h"
#include "OpenGL/WindowSubsystemInfo_OpenGL.h"

namespace JumaEngine
{
    inline WindowSubsystem_RenderAPIObject* createWindowSubsystemRenderAPIObject(const RenderAPI renderAPI)
    {
        switch (renderAPI)
        {
        case RenderAPI::Vulkan: return WindowSubsystemInfo<RenderAPI::Vulkan>::createRenderAPIObject();
        case RenderAPI::OpenGL: return WindowSubsystemInfo<RenderAPI::OpenGL>::createRenderAPIObject();
        default: ;
        }
        JUMA_LOG(error, JSTR("Unsupported render API"));
        return nullptr;
    }
}
