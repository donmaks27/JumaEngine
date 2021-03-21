// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"
#include "EngineContextObject.h"
#include "utils/log.h"
#include "window/WindowBase.h"
#include "render/RenderManagerBase.h"

namespace JumaEngine
{
    int32 Engine::startEngine(int argc, char** argv, WindowBase* window, RenderManagerBase* renderManager)
    {
#if _DEBUG
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

        _CrtMemState memoryState;
        _CrtMemCheckpoint(&memoryState);
#endif

        const int32 exitCode = startEngineInternal(argc, argv, window, renderManager);
        terminateEngine();
        JUMA_LOG(info, JTEXT("Engine stopped"));

#if _DEBUG
        _CrtMemDumpAllObjectsSince(&memoryState);
#endif

        return exitCode;
    }
    int32 Engine::startEngineInternal(int argc, char** argv, WindowBase* window, RenderManagerBase* renderManager)
    {
        JUMA_LOG(info, JTEXT("Start engine..."));

        int32 resultCode = ExitCode::OK;
        if (!initWindow(resultCode, window))
        {
            return resultCode;
        }
        if (!initRender(resultCode, renderManager))
        {
            return resultCode;
        }

        onInit();
        JUMA_LOG(correct, JTEXT("Initialization complete"));
        
        JUMA_LOG(info, JTEXT("Start engine loop..."));
        m_Window->onEngineLoopStart();

        bool firstFrame = true;
        while (!shouldStopEngine())
        {
            if (!firstFrame)
            {
                onUpdate(getDeltaTime());
                onPostUpdate();
            }
            else
            {
                firstFrame = false;
            }

            m_RenderManager->render();
            m_Window->onFrameRenderFinish();
        }

        JUMA_LOG(info, JTEXT("Stop engine..."));
        onStop();
        
        return ExitCode::OK;
    }

    bool JumaEngine::Engine::initWindow(int32& resultCode, WindowBase* window)
    {
        if (window == nullptr)
        {
            resultCode = ExitCode::EmptyWindowObject;
            return false;
        }

        m_Window = window;
        if (!m_Window->createWindow())
        {
            resultCode = ExitCode::FailWindowInit;
            return false;
        }

        return true;
    }
    bool Engine::initRender(int32& resultCode, RenderManagerBase* renderManager)
    {
        if (renderManager == nullptr)
        {
            resultCode = ExitCode::EmptyRenderManager;
            return false;
        }

        m_RenderManager = renderManager;
        registerEngineObject(m_RenderManager);
        if (!m_RenderManager->init())
        {
            resultCode = ExitCode::FailRenderManagerInit;
            return false;
        }

        return true;
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

    void Engine::terminateEngine()
    {
        if (m_Window != nullptr)
        {
            m_Window->termiante();
            m_Window = nullptr;
        }

        if (m_RenderManager != nullptr)
        {
            m_RenderManager->terminate();
            m_RenderManager = nullptr;
        }
    }

    void Engine::registerEngineObject(EngineContextObject* object)
    {
        if (object != nullptr)
        {
            object->m_OwnerEngine = this;
        	object->onRegister();
        }
    }
}
