// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"
#include "asset/mesh/vertexTypes/Vertex2D.h"
#include "subsystems/render/Image.h"
#include "subsystems/render/Material.h"
#include "subsystems/render/Shader.h"
#include "subsystems/render/VertexBuffer.h"
#include "subsystems/render/OpenGL/RenderSubsystem_OpenGL_GLFW.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan_GLFW.h"
#include "utils/jlog.h"

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
        m_RenderSubsystem = createObject<RenderSubsystem_Vulkan_GLFW>();
        if (m_RenderSubsystem == nullptr)
        {
            return false;
        }
        m_RenderSubsystem->initSubsystem();

        /*const uint8 imageData[4] = { 255, 0, 0, 255 };
        m_Image = m_RenderSubsystem->createImage();
        m_Image->init({ 1, 1 }, ImageFormat::R8G8B8A8, imageData);

        m_Shader = m_RenderSubsystem->createShader();
        m_Shader->init(JSTR("content/shaders/ui_texture"), {
            { JSTR("uTexture"), { 0, ShaderUniformType::Image, { ShaderStage::Fragment } } }
        });
        m_Material = m_RenderSubsystem->createMaterial();
        m_Material->init(m_Shader);
        m_Material->setUniformValue<ShaderUniformType::Image>(JSTR("uTexture"), m_Image);

        DefaultVertexBuffer defaultVertexBufferData;
        defaultVertexBufferData.vertices = {
            { { 0.0f, 0.0f, 0.0f } },
            { { 0.5f, 0.0f, 0.0f } },
            { { 0.0f, 0.5f, 0.0f } },
            { { 0.0f, 0.5f, 0.0f } },
            { { 0.5f, 0.0f, 0.0f } },
            { { 0.5f, 0.5f, 0.0f } }
        };
        VertexBufferDataBase* vertexBufferData = new VertexBufferData_Vertex2D();
        vertexBufferData->copyFromDefaultVertexBuffer(defaultVertexBufferData);
        m_VertexBuffer = m_RenderSubsystem->createVertexBuffer();
        m_VertexBuffer->init(vertexBufferData);
        delete vertexBufferData;*/

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
        m_VertexBuffer.reset();
        m_Material.reset();
        m_Shader.reset();
        m_Image.reset();

        m_RenderSubsystem->clear();
        delete m_RenderSubsystem;
        m_RenderSubsystem = nullptr;
    }

    void Engine::render(const RenderOptions& options)
    {
        /*m_Material->renderActivate(options);
        m_VertexBuffer->render(options);
        m_Material->renderDeactivate(options);*/
    }
}
