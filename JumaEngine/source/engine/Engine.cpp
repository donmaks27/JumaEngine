// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"

#include <chrono>

#include "subsystems/render/vertex/Vertex2D.h"
#include "subsystems/render/ImageOld.h"
#include "subsystems/render/MaterialOld.h"
#include "subsystems/render/ShaderOld.h"
#include "subsystems/render/OpenGL/RenderSubsystem_OpenGL_GLFW.h"
#include "subsystems/render/vertex/VertexBufferData.h"
#include "subsystems/render/Vulkan/ImageOld_Vulkan.h"
#include "subsystems/render/Vulkan/RenderSubsystem_Vulkan.h"
#include "jutils/jlog.h"
#include "jutils/jstringID.h"
#include "subsystems/render/Material.h"
#include "subsystems/render/Shader.h"
#include "subsystems/render/Texture.h"
#include "subsystems/render/VertexBuffer.h"
#include "subsystems/render/texture/TextureData.h"
#include "subsystems/window/Vulkan/WindowSubsystem_Vulkan_GLFW.h"

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

        m_WindowSubsytem = createObject<WindowSubsystem_Vulkan_GLFW>();
        if (m_WindowSubsytem == nullptr)
        {
            return false;
        }
        m_WindowSubsytem->initSubsystem();

        m_RenderSubsystem = createObject<RenderSubsystem_Vulkan>();
        if (m_RenderSubsystem == nullptr)
        {
            return false;
        }
        m_RenderSubsystem->initSubsystem();

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
        m_Texture->createRenderObject();

        m_Shader = createObject<Shader>();
        m_Shader->init(
            JSTR("content/shaders/ui_texture"), JSTR("content/shaders/ui_texture"),
            { { JSTR("uTexture"), ShaderUniform{ 0, ShaderUniformType::Texture, { ShaderStage::Fragment } } } }
        );
        m_Shader->createRenderObject();

        m_Material = createObject<Material>();
        m_Material->init(m_Shader);
        m_Material->setParamValue<ShaderUniformType::Texture>(JSTR("uTexture"), m_Texture);
        m_Material->createRenderObject();

        ImportedVertexBuffer defaultVertexBufferData;
        defaultVertexBufferData.vertices = {
            { { 0.0f, 0.0f, 0.0f } },
            { { 0.5f, 0.0f, 0.0f } },
            { { 0.0f, 0.5f, 0.0f } },
            { { 0.0f, 0.5f, 0.0f } },
            { { 0.5f, 0.0f, 0.0f } },
            { { 0.5f, 0.5f, 0.0f } }
        };
        VertexBufferDataBase* vertexData = new VertexBufferData<Vertex2D>;
        vertexData->copyFromImportedVertexBuffer(defaultVertexBufferData);

        m_VertexBuffer = createObject<VertexBuffer>();
        m_VertexBuffer->init(vertexData);
        m_VertexBuffer->createRenderObject();

        //const uint8 imageData[4] = { 255, 0, 0, 0 };
        //jshared_ptr<ImageOld> image = m_RenderSubsystem->createImage();
        //image->init({ 1, 1 }, TextureFormat::RGBA, imageData);

        //jshared_ptr<ShaderOld> shader = m_RenderSubsystem->createShaderOld();
        ////shader->init(JSTR("content/shaders/ui"));
        //shader->init(JSTR("content/shaders/ui_texture"), {
        //    { JSTR("uTexture"), ShaderUniform{ 0, ShaderUniformType::Texture, { ShaderStage::Fragment } } }
        //});
        //jshared_ptr<MaterialOld> material = m_RenderSubsystem->createMaterial();
        //material->init(shader);
        //material->setUniformValue<ShaderUniformType::Texture>(JSTR("uTexture"), image);

        //ImportedVertexBuffer defaultVertexBufferData;
        //defaultVertexBufferData.vertices = {
        //    { { 0.0f, 0.0f, 0.0f } },
        //    { { 0.5f, 0.0f, 0.0f } },
        //    { { 0.0f, 0.5f, 0.0f } },
        //    { { 0.0f, 0.5f, 0.0f } },
        //    { { 0.5f, 0.0f, 0.0f } },
        //    { { 0.5f, 0.5f, 0.0f } }
        //};
        //VertexBufferData<Vertex2D> vertexBufferData;
        //vertexBufferData.copyFromImportedVertexBuffer(defaultVertexBufferData);
        //jshared_ptr<VertexBufferOld> vertexBuffer = m_RenderSubsystem->createVertexBuffer();
        //vertexBuffer->init(&vertexBufferData);

        //m_RenderPrimitive = m_RenderSubsystem->createRenderPrimitive();
        //m_RenderPrimitive->init(vertexBuffer, material);

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
        m_RenderSubsystem->onEnginePreTerminate();

        delete m_VertexBuffer;
        delete m_Material;
        delete m_Shader;
        delete m_Texture;
        //m_RenderPrimitive.reset();

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
        //m_RenderPrimitive->render(options);
        m_Material->render(m_VertexBuffer, options);
    }
}
