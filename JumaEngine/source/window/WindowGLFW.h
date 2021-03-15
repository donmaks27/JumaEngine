// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "WindowBase.h"

struct GLFWwindow;

namespace JumaEngine
{
    class WindowGLFW : public WindowBase
    {
    public:
        WindowGLFW();
        virtual ~WindowGLFW() override;

        virtual double getDeltaTime() const override { return m_DeltaTime; }
        virtual bool shouldCloseWindow() const override;

        virtual bool init() override;
        virtual bool isInitialized() const override { return m_WindowObject != nullptr; }
        
        virtual void onFrameRenderFinish() override;

    private:

        GLFWwindow* m_WindowObject = nullptr;

        double m_LastTimeStamp = 0.0;
        double m_DeltaTime = 0.0;


        static void ErrorCallback(int code, const char* errorMessage);

        GLFWwindow* CreateWindow() const;
    };
}
