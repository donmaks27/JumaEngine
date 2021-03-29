// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"
#include "EngineContextObject.h"
#include "utils/log.h"
#include "window/WindowBase.h"
#include "render/RenderManagerBase.h"
#include "framework/gameObject/CameraComponent.h"
#include "framework/gameObject/EngineWorld.h"
#include "framework/gameObject/gameComponent/MeshComponent.h"
#include "framework/material/Material.h"
#include "render/vertexBuffer/VertexPosition.h"
#include "render/vertexBuffer/importer/VertexBufferImporterBase.h"
#include "utils/system_functions.h"

namespace JumaEngine
{
    void Engine::registerEngineObject(EngineContextObject* object)
    {
        if (object != nullptr)
        {
            object->m_OwnerEngine = this;
        	object->onRegister();
        }
    }

    bool Engine::startEngine(int argc, char** argv)
    {
#if _DEBUG
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

        _CrtMemState memoryState;
        _CrtMemCheckpoint(&memoryState);
#endif

        const bool result = startEngineInternal(argc, argv);
        terminateEngine();

#if _DEBUG
        _CrtMemDumpAllObjectsSince(&memoryState);
#endif

        return result;
    }
    bool Engine::startEngineInternal(int argc, char** argv)
    {
        JUMA_LOG(info, JTEXT("Start engine..."));
        if (initEngine())
        {
            JUMA_LOG(correct, JTEXT("Initialization complete"));

            JUMA_LOG(info, JTEXT("Start engine loop..."));
            startEngineLoop();

            JUMA_LOG(info, JTEXT("Stopping engine..."));
            stopEngine();
            JUMA_LOG(info, JTEXT("Engine stopped"));

            return true;
        }

        JUMA_LOG(error, JTEXT("Engnie initialization failed"));
        return false;
    }

    bool Engine::initEngine()
    {
        if (!initWindow() || !initRender())
        {
            return false;
        }

        onEngineInit();
        return true;
    }
    bool Engine::initWindow() const
    {
        return (m_Window != nullptr) && m_Window->init();
    }
    bool Engine::initRender() const
    {
        return (m_RenderManager != nullptr) && m_RenderManager->init();
    }

    void Engine::startEngineLoop()
    {
        onGameStart();

        bool firstFrame = true;
        while (!shouldStopEngine())
        {
            if (firstFrame)
            {
                firstFrame = false;
            }
            else
            {
                tick(getDeltaTime());
            }
			
			m_RenderManager->onRenderStart();
        	render();
        	
			m_Window->onRenderFinish();
        }
    }
    bool Engine::shouldStopEngine() const
    {
        return m_Window != nullptr ? m_Window->shouldCloseWindow() : true;
    }
    double Engine::getDeltaTime() const
    {
        return m_Window != nullptr ? m_Window->getDeltaTime() : 0.0;
    }

    void Engine::terminateEngine()
    {
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
    
    void Engine::onEngineInit()
    {
        m_World = createObject<EngineWorld>();

        m_Material = createObject<Material>();
        m_Material->setShaderName("content/shaders/testShader");

        SystemFunctions::importVertexBufferFile(m_World, "");
        m_Mesh = SystemFunctions::importMesh<Mesh, VertexBufferDataPosition>(m_World, JTEXT("Triangle"));
        m_Mesh->setMaterial(0, m_Material);

        MeshComponent* component = m_World->createSceneComponent<MeshComponent>();
        component->setMesh(m_Mesh);

        m_Camera = m_World->createSceneComponent<CameraComponent>();
    	m_Camera->setWorldLocation({ -50.0f, 0.0f, 0.0f });
    	//m_Camera->setWorldRotation({ 0.0f, 0.0f, 0.0f });
    }

    void Engine::onGameStart()
    {
    	if (m_World != nullptr)
    	{
    		m_World->onGameStarted();
    	}
    	
    	m_Window->onEngineLoopStart();
    }

    void Engine::tick(const double deltaTime)
    {
    	if (m_World != nullptr)
    	{
    		m_World->tick(deltaTime);
    	}
    }

    void Engine::render()
    {
    	if (m_World != nullptr)
    	{
    		m_World->render();
    	}
    }

    void Engine::stopEngine()
    {
        m_Camera = nullptr;
        if (m_World != nullptr)
        {
            delete m_World;
            m_World = nullptr;
        }

        if (m_Mesh != nullptr)
        {
            delete m_Mesh;
            m_Mesh = nullptr;
        }
        if (m_Material != nullptr)
        {
            delete m_Material;
            m_Material = nullptr;
        }
    }
}
