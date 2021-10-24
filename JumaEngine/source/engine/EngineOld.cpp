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
#include "asset/mesh/MeshFileImporterBase.h"
#include "asset/texture/TextureFileImporterBase.h"
#include "render/vertexBuffer/vertexType/Vertex3D_Normal_TexCoord.h"

namespace JumaEngine
{
    EngineOld::EngineOld()
    {
        m_MainThreadID = std::this_thread::get_id();
    }

    EngineContextObjectOld* EngineOld::createObject(const EngineContextObjectOld::ClassType* objectClass)
    {
        EngineContextObjectOld* object = objectClass != nullptr ? objectClass->createObject() : nullptr;
        if (object != nullptr)
        {
            registerEngineObject(object);
            return object;
        }
        return nullptr;
    }
    void EngineOld::registerEngineObject(EngineContextObjectOld* object)
    {
        if (object != nullptr)
        {
            object->m_OwnerEngine = this;
        	object->onRegister();
        }
    }

    bool EngineOld::startEngine(int argc, char** argv)
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
    bool EngineOld::startEngineInternal(int argc, char** argv)
    {
        JUMA_LOG(info, JSTR("Initialize engine..."));
		if (!initEngine())
		{
			JUMA_LOG(error, JSTR("Engnie initialization failed"));
			return false;
		}
		JUMA_LOG(correct, JSTR("Initialization complete"));
    	onEngineInit();

    	JUMA_LOG(info, JSTR("Start engine loop"));
    	startEngineLoop();
    	JUMA_LOG(info, JSTR("Engine loop finished"));

    	JUMA_LOG(info, JSTR("Stopping engine..."));
        stopEngine();
        JUMA_LOG(info, JSTR("Engine stopped"));

    	return true;
    }

    bool EngineOld::initEngine()
    {
		m_RenderManager = createObject<RenderManager_OpenGL>();
    	if (!m_RenderManager->isInit())
    	{
    		return false;
    	}
    	
    	return true;
    }
    void EngineOld::onEngineInit()
    {
		m_AssetsManager = createObject<AssetsManager>();
    	
        m_World = createObject<EngineWorld>();

        asset_ptr<Material> material = m_AssetsManager->createMaterial("content/shaders/testShaderTexCoords");
    	material->addMaterialParam<glm::mat4>("uProjection", glm::mat4(1));
    	material->addMaterialParam<glm::mat4>("uView", glm::mat4(1));
    	material->addMaterialParam<glm::mat4>("uModel", glm::mat4(1));

        const asset_ptr<TextureBase> texture = m_AssetsManager->createTexture(JSTR("JUMA"));
        m_TextureFileImporter->importFile(texture, "content/1.png");
        material->addMaterialParam<TextureBase>("uTexture", TextureShaderUniform{ texture, 0 });
    	material->finishInitialization();

        m_MeshFileImporter->importFile("content/SM_Cube.fbx");
        asset_ptr<Mesh> mesh = m_AssetsManager->createMesh(JSTR("Cube"));
        m_MeshFileImporter->copyMeshData<VertexBufferData3D_Normal_TexCoord>(mesh, JSTR("Cube"));
        mesh->setMaterial(0, m_AssetsManager->createMaterialInstance(material));

        MeshComponent* component = m_World->createSceneComponent<MeshComponent>();
        component->setMesh(mesh);
        //component->setWorldScale({ 0.3f, 0.3f, 0.3f });

        CameraComponent* camera = m_World->createSceneComponent<CameraComponent>();
        Rotation rotation = { -30.0f, 60.0f };
        camera->setWorldRotation(rotation);
        glm::vec3 location = -rotation.toDirection() * 5.0f;
        //location.z = 30.0f;
    	camera->setWorldLocation(location);

        m_RenderTarget = m_RenderManager->createRenderTargetDirect();
        m_RenderTarget->setCamera(camera);
        m_RenderManager->setWindowRenderTarget(m_RenderManager->getMainWindowID(), m_RenderTarget);
    }

    void EngineOld::startEngineLoop()
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
	
    void EngineOld::onEngineLoopStart()
    {
    	if (m_World != nullptr)
    	{
    		m_World->onGameStarted();
    	}
    }
	
    bool EngineOld::shouldStopEngine() const
    {
        return m_RenderManager != nullptr ? m_RenderManager->shouldCloseMainWindow() : true;
    }

    void EngineOld::tick(const double deltaTime)
    {
    	if (m_World != nullptr)
    	{
    		m_World->tick(deltaTime);
    	}
    }

    void EngineOld::render()
    {
    	m_RenderManager->startRender();
    }

    void EngineOld::stopEngine()
    {
        delete m_RenderTarget;
        m_RenderTarget = nullptr;

        delete m_World;
        m_World = nullptr;

		delete m_AssetsManager;
    	m_AssetsManager = nullptr;
    }
	
    void EngineOld::terminateEngine()
    {
    	delete m_MeshFileImporter;
    	m_MeshFileImporter = nullptr;

        if (m_RenderManager != nullptr)
        {
            m_RenderManager->terminate();
            delete m_RenderManager;
            m_RenderManager = nullptr;
        }
    }
}
