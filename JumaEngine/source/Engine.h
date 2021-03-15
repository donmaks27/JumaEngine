// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "window/WindowBase.h"

namespace JumaEngine
{
    class Engine
    {
    public:

        enum ExitCode
        {
            OK = 0,
            EmptyWindowObject = 1,
            FailWindowInit = 2
        };

        Engine();
        virtual ~Engine();

        int32 startEngine(int argc, char** argv, WindowBase* window);
        int32 startEngine(WindowBase* window) { return startEngine(0, nullptr, window); }

        WindowBase* getWindow() const { return m_Window; }

        bool shouldStopEngine() const;
        double getDeltaTime() const;

        virtual void onInit();
        virtual void onUpdate(double deltaTime);
        virtual void onPostUpdate();
        virtual void onStop();

    private:

        WindowBase* m_Window = nullptr;


        int32 startEngineInternal(int argc, char** argv, WindowBase* window);
    };
}
