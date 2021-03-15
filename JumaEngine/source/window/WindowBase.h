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

        virtual bool init() { return false; }
        virtual bool isInitialized() const { return false; }

        virtual bool shouldCloseWindow() const = 0;

        virtual double getDeltaTime() const { return 0.0; }

        virtual void onFrameRenderFinish() = 0;
    };
}
