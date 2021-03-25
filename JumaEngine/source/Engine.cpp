// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"
#include "EngineContextObject.h"
#include "utils/log.h"
#include "window/WindowBase.h"
#include "render/RenderManagerBase.h"
#include "framework/gameObject/Camera.h"
#include "render/vertexBuffer/importer/VertexBufferImporterBase.h"

namespace JumaEngine
{
    int32 Engine::startEngine(int argc, char** argv)
    {
#if _DEBUG
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

        _CrtMemState memoryState;
        _CrtMemCheckpoint(&memoryState);
#endif

        const int32 exitCode = startEngineInternal(argc, argv);
        terminateEngine();
        JUMA_LOG(info, JTEXT("Engine stopped"));

#if _DEBUG
        _CrtMemDumpAllObjectsSince(&memoryState);
#endif

        return exitCode;
    }
    int32 Engine::startEngineInternal(int argc, char** argv)
    {
        JUMA_LOG(info, JTEXT("Start engine..."));

        int32 resultCode = ExitCode::OK;
        if (!initWindow(resultCode))
        {
            return resultCode;
        }
        if (!initRender(resultCode))
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

    bool JumaEngine::Engine::initWindow(int32& resultCode) const
    {
        if (m_Window == nullptr)
        {
            resultCode = ExitCode::EmptyWindowObject;
            return false;
        }
        if (!m_Window->createWindow())
        {
            resultCode = ExitCode::FailWindowInit;
            return false;
        }
        return true;
    }
    bool Engine::initRender(int32& resultCode)
    {
        if (m_RenderManager == nullptr)
        {
            resultCode = ExitCode::EmptyRenderManager;
            return false;
        }

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
        m_Camera = createObject<Camera>();
    	m_Camera->setWorldLocation({ -50.0f, 0.0f, 0.0f });
    	//m_Camera->setWorldRotation({ 0.0f, 0.0f, 0.0f });
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
        if (m_Camera != nullptr)
        {
            delete m_Camera;
            m_Camera = nullptr;
        }

        if (m_VertexBufferImporter != nullptr)
        {
            delete m_VertexBufferImporter;
            m_VertexBufferImporter = nullptr;
        }

        if (m_Window != nullptr)
        {
            m_Window->termiante();
            delete m_Window;
            m_Window = nullptr;
        }

        if (m_RenderManager != nullptr)
        {
            m_RenderManager->terminate();
            delete m_RenderManager;
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
