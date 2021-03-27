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
        WindowGLFW() = default;
        virtual ~WindowGLFW() override;

        virtual bool isInit() const override { return m_WindowObject != nullptr; }
        virtual void termiante() override;
        
        virtual void onEngineLoopStart() override;
        virtual void onFrameRenderFinish() override;

        virtual double getDeltaTime() const override { return m_DeltaTime; }
        virtual bool shouldCloseWindow() const override;

    protected:
    	
        virtual bool isSupportedRenderAPI(const RenderAPI api) const override { return api == RenderAPI::OpenGL; }
        virtual void initInternal() override;

        virtual void updateWindowTitle() override;
        virtual void updateWindowSize() override;

    private:

        GLFWwindow* m_WindowObject = nullptr;

        double m_LastTimeStamp = 0.0;
        double m_DeltaTime = 0.0;


        static void errorCallback(int code, const char* errorMessage);

        void updateDeltaTime();

        void terminateInternal();
    };
}
