// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"

namespace JumaEngine
{
    Engine::Engine()
    {
    }
    Engine::~Engine()
    {
    }

    int32 Engine::startEngine(int argc, char** argv, WindowBase* window)
    {
#if _DEBUG
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

        _CrtMemState memoryState;
        _CrtMemCheckpoint(&memoryState);
#endif

        const int32 exitCode = startEngineInternal(argc, argv, window);

#if _DEBUG
        _CrtMemDumpAllObjectsSince(&memoryState);
#endif

        return exitCode;
    }
    int32 Engine::startEngineInternal(int argc, char** argv, WindowBase* window)
    {
        if (window == nullptr)
        {
            return ExitCode::EmptyWindowObject;
        }
        m_Window = window;

        if (!m_Window->init())
        {
            return ExitCode::FailWindowInit;
        }

        onInit();
        while (!shouldStopEngine())
        {
            onUpdate(getDeltaTime());
            onPostUpdate();

            m_Window->onFrameRenderFinish();
        }
        onStop();
        
        return ExitCode::OK;
    }

    bool Engine::shouldStopEngine() const
    {
        return m_Window != nullptr ? m_Window->shouldCloseWindow() : true;
    }
    double Engine::getDeltaTime() const
    {
        return m_Window != nullptr ? m_Window->getDeltaTime() : 0.0;
    }

    void Engine::onInit()
    {
    }
    void Engine::onUpdate(double deltaTime)
    {
    }
    void Engine::onPostUpdate()
    {
    }
    void Engine::onStop()
    {
    }

}
