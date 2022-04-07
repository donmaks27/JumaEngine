// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"

#include <chrono>

#include "jutils/jlog.h"
#include "jutils/jstringID.h"
#include "subsystems/render/Material.h"
#include "subsystems/render/RenderOptions.h"
#include "subsystems/render/RenderPipeline.h"
#include "subsystems/render/Shader.h"
#include "subsystems/render/Texture.h"
#include "subsystems/render/VertexBuffer.h"
#include "subsystems/render/RenderSubsystem.h"
#include "subsystems/render/RenderTarget.h"
#include "subsystems/render/texture/TextureData.h"
#include "subsystems/render/vertex/Vertex2D.h"
#include "subsystems/render/vertex/Vertex2D_TexCoord.h"
#include "subsystems/render/vertex/VertexBufferData.h"
#include "subsystems/window/WindowSubsystem.h"

namespace JumaEngine
{
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
        	object->onRegistered();
        }
    }

    bool Engine::startEngine()
    {
        if (m_Started)
        {
            return false;
        }

        m_Started = true;
    	
#if JDEBUG && _WIN32
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

        _CrtMemState memoryState;
        _CrtMemCheckpoint(&memoryState);
#endif

        const bool result = startEngineInternal();

#if JDEBUG && _WIN32
        _CrtMemDumpAllObjectsSince(&memoryState);
#endif
        
		m_Started = false;
        return result;
    }
    bool Engine::startEngineInternal()
    {
        JUMA_LOG(info, JSTR("Initializing engine..."));
        if (!initEngine())
        {
            JUMA_LOG(error, JSTR("Failed to initialize Engine!"));
            return false;
        }
        JUMA_LOG(correct, JSTR("Engine initialized"));

        JUMA_LOG(info, JSTR("Starting engine loop..."));
        startEngineLoop();
        JUMA_LOG(info, JSTR("Engine loop finished"));

        terminate();
        JUMA_LOG(info, JSTR("Engine terminated"));

        return true;
    }
    bool Engine::initEngine()
    {
        jstring_hash_table::CreateInstance();

        m_WindowSubsytem = createObject<WindowSubsystem>();
        if (m_WindowSubsytem == nullptr)
        {
            return false;
        }
        m_WindowSubsytem->initSubsystem();
        if (!m_WindowSubsytem->isValid())
        {
            return false;
        }

        m_RenderSubsystem = createObject<RenderSubsystem>();
        if (m_RenderSubsystem == nullptr)
        {
            return false;
        }
        m_RenderSubsystem->initSubsystem();
        if (!m_RenderSubsystem->isValid())
        {
            return false;
        }
        
        m_WindowSubsytem->createRenderAPIObject();
        m_RenderSubsystem->createRenderAPIObject();

        TextureData* textureData = new TextureData();
        textureData->init(
            { 2, 2 }, TextureFormat::RGBA, 
            new uint8[16]
            {
                255, 0, 0, 0,
                0, 255, 0, 0,
                0, 0, 255, 0,
                0, 0, 0, 0
            }
        );

        m_Texture = createObject<Texture>();
        m_Texture->init(textureData);
        m_Texture->createRenderAPIObject();

        m_Shader = createObject<Shader>();
        m_Shader->init(
            JSTR("content/shaders/ui_texture"), JSTR("content/shaders/ui_texture"),
            { { JSTR("uTexture"), ShaderUniform{ 0, ShaderUniformType::Texture, { ShaderStage::Fragment } } } }
        );
        /*m_Shader->init(
            JSTR("content/shaders/ui"), JSTR("content/shaders/ui")
        );*/
        m_Shader->createRenderAPIObject();

        m_Material = createObject<Material>();
        m_Material->init(m_Shader);
        m_Material->setParamValue<ShaderUniformType::Texture>(JSTR("uTexture"), m_Texture);
        m_Material->createRenderAPIObject();

        VertexBufferData<Vertex2D>* vertexData = new VertexBufferData<Vertex2D>();
        vertexData->setVertices({
            { { 0.0f, 0.0f } },
            { { 0.5f, 0.0f } },
            { { 0.0f, 0.5f } },
            { { 0.0f, 0.5f } },
            { { 0.5f, 0.0f } },
            { { 0.5f, 0.5f } }
        });
        m_VertexBuffer = createObject<VertexBuffer>();
        m_VertexBuffer->init(vertexData);
        m_VertexBuffer->createRenderAPIObject();

        m_RenderTarget = createObject<RenderTarget>();
        m_RenderTarget->init(TextureFormat::RGBA, { 800, 600 });
        m_RenderTarget->createRenderAPIObject();

        m_ShaderPP = createObject<Shader>();
        m_ShaderPP->init(
            JSTR("content/shaders/ui_postProcess"), JSTR("content/shaders/ui_texture"),
            { { JSTR("uTexture"), ShaderUniform{ 0, ShaderUniformType::RenderTarget, { ShaderStage::Fragment } } } }
        );
        m_ShaderPP->createRenderAPIObject();

        m_MaterialPP = createObject<Material>();
        m_MaterialPP->init(m_ShaderPP);
        m_MaterialPP->setParamValue<ShaderUniformType::RenderTarget>(JSTR("uTexture"), m_RenderTarget);
        m_MaterialPP->createRenderAPIObject();

        VertexBufferData<Vertex2D_TexCoord>* ppVertexData = new VertexBufferData<Vertex2D_TexCoord>();
        ppVertexData->setVertices({
            { { -1.0f, -1.0f }, { 0.0f, 0.0f } },
            { {  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { { -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { { -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { {  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { {  1.0f,  1.0f }, { 1.0f, 1.0f } }
        });
        m_VertexBufferPP = createObject<VertexBuffer>();
        m_VertexBufferPP->init(ppVertexData);
        m_VertexBufferPP->createRenderAPIObject();

        RenderPipeline* renderPipeline = m_RenderSubsystem->getRenderPipeline();
        renderPipeline->addRenderTargetPipelineStage(JSTR("MainPass"), m_RenderTarget);
        renderPipeline->addWindowPipelineStage(JSTR("WindowPass"), m_RenderSubsystem->getMainWindowID());
        renderPipeline->addPipelineStageDependency(JSTR("WindowPass"), JSTR("MainPass"));
        renderPipeline->validatePipelineQueue();
        return true;
    }

    void Engine::startEngineLoop()
    {
        std::chrono::time_point<std::chrono::steady_clock> lastTimestamp = std::chrono::steady_clock::now();
        while (!m_RenderSubsystem->shouldCloseMainWindow())
        {
            const std::chrono::time_point<std::chrono::steady_clock> timestamp = std::chrono::steady_clock::now();
    		const double deltaTime = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(timestamp - lastTimestamp).count()) / 1000000.0;
            lastTimestamp = timestamp;

            m_RenderSubsystem->render();
        }
    }

    void Engine::terminate()
    {
        m_RenderSubsystem->waitForRenderFinish();

        delete m_VertexBufferPP;
        delete m_MaterialPP;
        delete m_ShaderPP;

        delete m_RenderTarget;
        delete m_VertexBuffer;
        delete m_Material;
        delete m_Shader;
        delete m_Texture;

        m_RenderSubsystem->clear();
        delete m_RenderSubsystem;
        m_RenderSubsystem = nullptr;

        m_WindowSubsytem->clear();
        delete m_WindowSubsytem;
        m_WindowSubsytem = nullptr;

        jstring_hash_table::ClearInstance();
    }

    void Engine::render(const RenderOptions* options)
    {
        if (options->renderTargetName == JSTR("MainPass"))
        {
            m_Material->render(m_VertexBuffer, options);
        }
        else
        {
            m_MaterialPP->render(m_VertexBufferPP, options);
        }
    }
}
