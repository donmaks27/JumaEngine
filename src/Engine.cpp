// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../include/JumaEngine/Engine.h"

#include <JumaRE/RenderEngineImpl.h>
#include <JumaRE/RenderPipeline.h>

#include "../include/JumaEngine/subsystems/shaders/ShadersSubsystem.h"
#include "../include/JumaEngine/subsystems/textures/TexturesSubsystem.h"

namespace JumaEngine
{
    EngineContextObject* Engine::registerObjectInternal(EngineContextObject* object)
    {
        if (object != nullptr)
        {
            object->m_Engine = this;
        }
        return object;
    }
    EngineContextObject* Engine::createObject(const EngineClass* engineClass)
    {
        return engineClass != nullptr ? registerObjectInternal(engineClass->createBaseObject()) : nullptr;
    }

    bool Engine::init(const EngineSubclass<GameInstance>& gameInstanceClass)
    {
        if (m_GameInstance != nullptr)
        {
            JUTILS_LOG(warning, JSTR("Engine already initialized!"));
            return false;
        }

        if (!initEngine())
        {
            JUTILS_LOG(error, JSTR("Failed to init engine"));
            return false;
        }

        m_GameInstance = createObject(gameInstanceClass);
        if (!initGameInstance())
        {
            JUTILS_LOG(error, JSTR("Failed to init game instance"));
            m_GameInstance->clear();
            delete m_GameInstance;
            m_GameInstance = nullptr;
            clear();
            return false;
        }

        if (!initRenderEngine())
        {
            JUTILS_LOG(error, JSTR("Failed to init render engine"));
            clear();
            return false;
        }

        if (!m_GameInstance->initRenderData())
        {
            JUTILS_LOG(error, JSTR("Failed to init render data of game instance"));
            clear();
            return false;
        }
        return true;
    }
    bool Engine::initEngine()
    {
        return true;
    }
    bool Engine::initGameInstance()
    {
        return m_GameInstance->init();
    }
    bool Engine::initRenderEngine()
    {
        m_RenderEngine = JumaRE::CreateRenderEngine(m_InitialRenderAPI);
        if (m_RenderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create render engine ({})"), m_InitialRenderAPI);
            return false;
        }
        if (!m_RenderEngine->init(m_InitialRenderEngineWindow))
        {
            JUTILS_LOG(error, JSTR("Failed to init render engine ({})"), m_InitialRenderAPI);
            delete m_RenderEngine;
            m_RenderEngine = nullptr;
            return false;
        }
        JUTILS_LOG(info, JSTR("Render engine initialized ({})"), m_InitialRenderAPI);

        m_RenderEngine->registerVertexComponent(JSTR("position2D"), { JumaRE::VertexComponentType::Vec2, 0 });
        m_RenderEngine->registerVertexComponent(JSTR("textureCoords"), { JumaRE::VertexComponentType::Vec2, 1 });

        if (createSubsystem<ShadersSubsystem>() == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to init shaders subsystem"));
            return false;
        }
        if (createSubsystem<TexturesSubsystem>() == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to init textures subsystem"));
            return false;
        }
        return true;
    }

    void Engine::start()
    {
        JUTILS_LOG(info, JSTR("Initializing engine loop..."));
        if (!initEngineLoop())
        {
            JUTILS_LOG(error, JSTR("Failed to init engine loop"));
            return;
        }

        JUTILS_LOG(info, JSTR("Starting engine loop..."));
        startEngineLoop();
        JUTILS_LOG(info, JSTR("Stopping engine loop..."));
        stopEngineLoop();
        JUTILS_LOG(info, JSTR("Engine loop stopped"));
    }
    bool Engine::initEngineLoop()
    {
        if (m_InitialGameInstanceRenderTarger == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invalid game instance render target"));
            return false;
        }
        m_GameInstance->setupRenderTarget(m_InitialGameInstanceRenderTarger);
        return true;
    }
    void Engine::startEngineLoop()
    {
        while (update())
        {
            if (!m_RenderEngine->getRenderPipeline()->render())
            {
                JUTILS_LOG(error, JSTR("Render failed"));
                break;
            }
            m_RenderEngine->getWindowController()->updateWindows();
        }

        getRenderEngine()->getRenderPipeline()->waitForRenderFinished();
    }
    bool Engine::update()
    {
        if (m_RenderEngine->getWindowController()->isMainWindowClosed())
        {
            return false;
        }
        return true;
    }
    void Engine::stopEngineLoop()
    {
    }

    void Engine::clear()
    {
        if (m_GameInstance != nullptr)
        {
            m_GameInstance->clearRenderData();
            clearRenderEngine();

            m_GameInstance->clear();
            delete m_GameInstance;
            m_GameInstance = nullptr;
        }
        clearEngine();
    }
    void Engine::clearRenderEngine()
    {
        if (m_RenderEngine != nullptr)
        {
            m_RenderEngine->clear();
            delete m_RenderEngine;
            m_RenderEngine = nullptr;
        }
    }
    void Engine::clearEngine()
    {
        for (const auto& subsystem : m_EngineSubsystems)
        {
            subsystem.value->clearSubsystem();
            delete subsystem.value;
        }
        m_EngineSubsystems.clear();
    }

    EngineSubsystem* Engine::createSubsystem(const EngineSubclass<EngineSubsystem>& subsystemClass)
    {
        if (subsystemClass == nullptr)
        {
            return nullptr;
        }
        EngineSubsystem* const* subsystemPtr = m_EngineSubsystems.find(subsystemClass);
        if (subsystemPtr != nullptr)
        {
            return *subsystemPtr;
        }
        EngineSubsystem* subsystem = createObject(subsystemClass);
        if (subsystem == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create subsystem {}"), subsystemClass->getClassName());
            return nullptr;
        }

        m_EngineSubsystems.add(subsystemClass, subsystem);
        if (!subsystem->initSubsystem())
        {
            JUTILS_LOG(error, JSTR("Failed to init subsystem {}"), subsystemClass->getClassName());
            m_EngineSubsystems.remove(subsystemClass);
            delete subsystem;
            return nullptr;
        }
        return subsystem;
    }
    EngineSubsystem* Engine::getSubsystem(const EngineSubclass<EngineSubsystem>& subsystemClass) const
    {
        if (subsystemClass == nullptr)
        {
            return nullptr;
        }
        EngineSubsystem* const* subsystemPtr = m_EngineSubsystems.find(subsystemClass);
        return subsystemPtr != nullptr ? *subsystemPtr : nullptr;
    }
}
