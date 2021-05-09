// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"
#include <chrono>
#include "EngineContextObject.h"
#include "asset/AssetsManager.h"
#include "utils/jlog.h"
#include "framework/gameObject/EngineWorld.h"
#include "framework/gameObject/gameComponent/CameraComponent.h"
#include "framework/gameObject/gameComponent/MeshComponent.h"
#include "asset/material/Material.h"
#include "asset/material/MaterialInstance.h"
#include "asset/mesh/Mesh.h"
#include "render/RenderManagerImpl.h"
#include "render/renderTarget/RenderTargetDirectBase.h"
#include "render/vertexBuffer/VertexPosition.h"
#include "render/vertexBuffer/importer/MeshImporterBase.h"

namespace JumaEngine
{
    Engine::Engine()
    {
        m_MainThreadID = std::this_thread::get_id();
    }

    EngineContextObject* Engine::createObject(const EngineContextObject::ClassType* objectClass)
    {
        EngineContextObject* object = objectClass != nullptr ? objectClass->createObject() : nullptr;
        if (object != nullptr)
        {
            registerEngineObject(object);
            return object;
        }
        return nullptr;
    }
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
		if (m_EngineStarted)
		{
			return false;
		}

		m_EngineStarted = true;
    	
#if JDEBUG && _WIN32
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

        _CrtMemState memoryState;
        _CrtMemCheckpoint(&memoryState);
#endif

        const bool result = startEngineInternal(argc, argv);
        terminateEngine();

#if JDEBUG && _WIN32
        _CrtMemDumpAllObjectsSince(&memoryState);
#endif

		m_EngineStarted = false;
        return result;
    }
    bool Engine::startEngineInternal(int argc, char** argv)
    {
        JUMA_LOG(info, JTEXT("Initialize engine..."));
		if (!initEngine())
		{
			JUMA_LOG(error, JTEXT("Engnie initialization failed"));
			return false;
		}
		JUMA_LOG(correct, JTEXT("Initialization complete"));
    	onEngineInit();

    	JUMA_LOG(info, JTEXT("Start engine loop"));
    	startEngineLoop();
    	JUMA_LOG(info, JTEXT("Engine loop finished"));

    	JUMA_LOG(info, JTEXT("Stopping engine..."));
        stopEngine();
        JUMA_LOG(info, JTEXT("Engine stopped"));

    	return true;
    }

    bool Engine::initEngine()
    {
		m_RenderManager = createObject<RenderManager_OpenGL>();
    	if (!m_RenderManager->init())
    	{
    		return false;
    	}
    	
    	return true;
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

        m_MeshImporter->importFile("");
        m_Mesh = createObject<Mesh>();
        m_Mesh->initMesh(m_MeshImporter->createVertexBuffersForMesh(JTEXT("Triangle"), jclass_type<VertexBufferDataPosition>()));
        m_Mesh->setMaterial(0, m_AssetsManager->createMaterialInstance(material));

        MeshComponent* component = m_World->createSceneComponent<MeshComponent>();
        component->setMesh(m_Mesh);

        CameraComponent* camera = m_World->createSceneComponent<CameraComponent>();
    	//m_Camera->setWorldLocation({ -50.0f, 0.0f, 0.0f });
    	//m_Camera->setWorldRotation({ 0.0f, 0.0f, 0.0f });

        m_RenderTarget = m_RenderManager->createRenderTargetDirect();
        m_RenderTarget->setCamera(camera);
        m_RenderManager->setWindowRenderTarget(m_RenderManager->getMainWindowID(), m_RenderTarget);

        const window_id windowID = m_RenderManager->createWindow(glm::uvec2(800, 600), JTEXT("Second window"));
        m_RenderManager->setWindowRenderTarget(windowID, m_RenderTarget);
    }

    void Engine::startEngineLoop()
    {
        onEngineLoopStart();

    	std::chrono::time_point<std::chrono::steady_clock> lastTimeStamp = std::chrono::steady_clock::now();
    	
		render();
    	while (!shouldStopEngine())
    	{
    		const std::chrono::time_point<std::chrono::steady_clock> timeStamp = std::chrono::steady_clock::now();
    		const double deltaTime = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(timeStamp - lastTimeStamp).count()) / 1000000.0;
    		lastTimeStamp = timeStamp;

    		tick(deltaTime);
    		render();
    	}
    }
	
    void Engine::onEngineLoopStart()
    {
    	if (m_World != nullptr)
    	{
    		m_World->onGameStarted();
    	}
    }
	
    bool Engine::shouldStopEngine() const
    {
        return m_RenderManager != nullptr ? m_RenderManager->shouldCloseMainWindow() : true;
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
    	m_RenderManager->startRender();
    }

    void Engine::stopEngine()
    {
        delete m_RenderTarget;
        m_RenderTarget = nullptr;

        delete m_World;
        m_World = nullptr;

		delete m_Mesh;
        m_Mesh = nullptr;

		delete m_AssetsManager;
    	m_AssetsManager = nullptr;
    }
	
    void Engine::terminateEngine()
    {
    	delete m_MeshImporter;
    	m_MeshImporter = nullptr;

        if (m_RenderManager != nullptr)
        {
            m_RenderManager->terminate();
            delete m_RenderManager;
            m_RenderManager = nullptr;
        }
    }
}
