// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    class WindowBase
    {
    public:
        WindowBase();
        virtual ~WindowBase();

        virtual bool createWindow() = 0;
        virtual bool isWindowCreated() const = 0;

        virtual void setWindowTitle(const char* title) = 0;

        virtual void onEngineLoopStart() {}

        virtual double getDeltaTime() const = 0;
        virtual bool shouldCloseWindow() const = 0;

        virtual void onFrameRenderFinish() = 0;

        virtual void termiante() {}
    };
}
