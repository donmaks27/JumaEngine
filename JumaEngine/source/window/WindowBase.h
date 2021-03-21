// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "glm/vec2.hpp"

namespace JumaEngine
{
    class WindowBase
    {
    public:
        WindowBase() = default;
        virtual ~WindowBase() = default;

        virtual bool createWindow() = 0;
        virtual bool isWindowCreated() const = 0;

        virtual void onEngineLoopStart() {}

        virtual double getDeltaTime() const = 0;
        virtual bool shouldCloseWindow() const = 0;

        virtual void onFrameRenderFinish() = 0;

        virtual void termiante() {}

        virtual void setWindowTitle(const char* title) = 0;

        void setWindowSize(const glm::uvec2& windowSize);
        glm::uvec2 getWindowSize() const { return m_WindowSize; }
        float getWindowAspectRatio() const;

    protected:

        glm::uvec2 m_WindowSize = glm::uvec2(0, 0);


        virtual void onWindowSizeChanged() {}
    };
}
