// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"
#include <chrono>
#include "EngineContextObject.h"
#include "asset/AssetsManager.h"
#include "utils/log.h"
#include "framework/gameObject/EngineWorld.h"
#include "framework/gameObject/gameComponent/CameraComponent.h"
#include "framework/gameObject/gameComponent/MeshComponent.h"
#include "asset/material/Material.h"
#include "asset/material/MaterialInstance.h"
#include "asset/mesh/Mesh.h"
#include "render/RenderManagerImpl.h"
#include "render/vertexBuffer/VertexPosition.h"
#include "render/vertexBuffer/importer/MeshImporterBase.h"
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
        if (!initRender())
        {
            return false;
        }

        onEngineInit();
        return true;
    }
    bool Engine::initRender()
    {
        m_RenderManager = new RenderManager_OpenGL();
        return m_RenderManager->init();
    }

    void Engine::startEngineLoop()
    {
        onGameStart();

        bool firstFrame = true;
        std::chrono::time_point<std::chrono::steady_clock> time1 = std::chrono::steady_clock::now();
        std::chrono::time_point<std::chrono::steady_clock> time2 = time1;
        while (!shouldStopEngine())
        {
            if (firstFrame)
            {
                firstFrame = false;
            }
            else
            {
                time1 = time2;
                time2 = std::chrono::steady_clock::now();
                const double time = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1).count()) / 1000000.0;
                tick(time);
            }
        	postTick();
   
			m_RenderManager->startRender();
        	render();
        	m_RenderManager->finishRender();
        }
    }
    bool Engine::shouldStopEngine() const
    {
        return m_RenderManager != nullptr ? m_RenderManager->shouldCloseMainWindow() : true;
    }
    double Engine::getDeltaTime() const
    {
        //return m_Window != nullptr ? m_Window->getDeltaTime() : 0.0;
        return 0.0;
    }

    void Engine::terminateEngine()
    {
        if (m_VertexBufferImporter != nullptr)
        {
            delete m_VertexBufferImporter;
            m_VertexBufferImporter = nullptr;
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
		m_AssetsManager = createObject<AssetsManager>();
    	
        m_World = createObject<EngineWorld>();

        asset_ptr<Material> material = m_AssetsManager->createMaterial("content/shaders/testShader");
    	material->addMaterialParam("uProjection", glm::mat4(1));
    	material->addMaterialParam("uView", glm::mat4(1));
    	material->addMaterialParam("uModel", glm::mat4(1));
    	material->finishInitialization();

        SystemFunctions::importVertexBufferFile(m_World, "");
        m_Mesh = SystemFunctions::importMesh(m_World, JTEXT("Triangle"), CLASS_TYPE(Mesh), CLASS_TYPE(VertexBufferDataPosition));
        m_Mesh->setMaterial(0, m_AssetsManager->createMaterialInstance(material));

        MeshComponent* component = m_World->createSceneComponent<MeshComponent>();
        component->setMesh(m_Mesh);

        m_Camera = m_World->createSceneComponent<CameraComponent>();
    	//m_Camera->setWorldLocation({ -50.0f, 0.0f, 0.0f });
    	//m_Camera->setWorldRotation({ 0.0f, 0.0f, 0.0f });
    }

    void Engine::onGameStart()
    {
    	if (m_World != nullptr)
    	{
    		m_World->onGameStarted();
    	}
    }

    void Engine::tick(const double deltaTime)
    {
    	if (m_World != nullptr)
    	{
    		m_World->tick(deltaTime);
    	}
    }
    void Engine::postTick()
    {
    	if (m_World != nullptr)
    	{
    		m_World->postTick();
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

    	if (m_AssetsManager != nullptr)
    	{
    		delete m_AssetsManager;
    		m_AssetsManager = nullptr;
    	}
    }
}
