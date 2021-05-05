// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "glm/vec2.hpp"
#include <thread>
#include <atomic>

namespace JumaEngine
{
    class RenderTargetBase;
    class CameraComponent;

    class WindowDescriptionBase
    {
    public:
        WindowDescriptionBase() = default;
        virtual ~WindowDescriptionBase() = default;

        jstring windowTitle;
        glm::uvec2 windowSize = { 0, 0 };

        std::thread windowThread;
        std::atomic_bool windowActive = false;
        std::atomic_bool windowShouldStartRender = false;
        std::atomic_bool windowRenderFinish = false;

        RenderTargetBase* windowRenderTarget = nullptr;

        CameraComponent* activeCamera = nullptr;
    };
}
