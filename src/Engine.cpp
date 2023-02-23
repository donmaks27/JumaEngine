﻿// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/Engine.h"

#include <chrono>
#include <JumaRE/RenderEngineImpl.h>
#include <JumaRE/RenderPipeline.h>
#include <jutils/configs/ini_parser.h>

#include "JumaEngine/assets/AssetsEngineSubsystem.h"
#include "JumaEngine/render/RenderEngineSubsystem.h"
#include "JumaEngine/widgets/WidgetsCreator.h"

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
        jmap<jstring, jmap<jstring, jstring>> configData = ini::parseFile(JSTR("config/engine.ini"));
        const jmap<jstring, jstring>* engineConfigData = configData.find(JSTR("General"));
        if (engineConfigData != nullptr)
        {
	        const jstring* contentEngine = engineConfigData->find(JSTR("contentFolderEngine"));
            if (contentEngine != nullptr)
            {
	            m_EngineContentDirectory = *contentEngine;
            }
	        const jstring* content = engineConfigData->find(JSTR("contentFolderGame"));
            if (content != nullptr)
            {
	            m_GameContentDirectory = *content;
            }
        }
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

        RenderEngineSubsystem* renderSubsystem = createSubsystem<RenderEngineSubsystem>();
        if (renderSubsystem == nullptr)
        {
	        JUTILS_LOG(error, JSTR("Failed to init render engine subsystem"));
            return false;
        }
        if (createSubsystem<AssetsEngineSubsystem>() == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to init assets engine subsystem"));
            return false;
        }

        m_EngineWidgetCreator = createObject<WidgetsCreator>();
        InitializeLogicObject(m_EngineWidgetCreator);

        renderSubsystem->createProxyWindowRenderTargets();
        return true;
    }

    void Engine::start()
    {
        JUTILS_LOG(info, JSTR("Starting engine loop..."));
        if (!onEngineLoopStarting())
        {
            JUTILS_LOG(error, JSTR("Failed to start engine loop"));
            return;
        }
        
        JUTILS_LOG(info, JSTR("Engine loop started"));
        onEngineLoopStarted();
        while (!shouldStopEngineLoop())
        {
            static std::chrono::time_point<std::chrono::steady_clock> prevTimePoint = std::chrono::steady_clock::now();
	        const std::chrono::time_point<std::chrono::steady_clock> currentTimePoint = std::chrono::steady_clock::now();
	        const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTimePoint - prevTimePoint);
	        prevTimePoint = prevTimePoint + duration;
	        const float deltaTime = static_cast<float>(duration.count()) / 1000000.0f;

            update(deltaTime);
            postUpdate();
            if (!m_RenderEngine->render())
            {
                JUTILS_LOG(error, JSTR("Render failed"));
                break;
            }
        }
        getRenderEngine()->getRenderPipeline()->waitForRenderFinished();
        
        JUTILS_LOG(info, JSTR("Stopping engine loop..."));
        onEngineLoopStopping();
        JUTILS_LOG(info, JSTR("Engine loop stopped"));

        clear();
    }
    bool Engine::onEngineLoopStarting()
    {
        JumaRE::RenderTarget* gameInstanceRT = getGameInstanceRenderTarget();
        if (gameInstanceRT == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invalid game instance render target"));
            return false;
        }
        
        m_GameInstance->setupRenderTarget(gameInstanceRT);
        return true;
    }
    void Engine::onEngineLoopStarted()
    {
        StartLogicObject(m_EngineWidgetCreator);
    }
    bool Engine::shouldStopEngineLoop()
    {
        return (m_RenderEngine != nullptr) && m_RenderEngine->getWindowController()->isMainWindowClosed();
    }
    void Engine::update(const float deltaTime)
    {
        UpdateLogicObject(m_EngineWidgetCreator, deltaTime);
    }
    void Engine::postUpdate()
    {
        PreRenderLogicObject(m_EngineWidgetCreator);
    }
    void Engine::onEngineLoopStopping()
    {
        getSubsystem<RenderEngineSubsystem>()->destroyProxyWindowRenderTargets();
        DestroyLogicObject(m_EngineWidgetCreator);
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
        if (m_EngineWidgetCreator != nullptr)
        {
	        delete m_EngineWidgetCreator;
            m_EngineWidgetCreator = nullptr;
        }

        EngineSubsystem* renderSubsystem = nullptr;
        for (const auto& subsystem : m_EngineSubsystems)
        {
            if (subsystem.key != RenderEngineSubsystem::GetClassStatic())
            {
	            subsystem.value->clearSubsystem();
            }
            else
            {
	            renderSubsystem = subsystem.value;
            }
        }
        if (renderSubsystem != nullptr)
        {
	        renderSubsystem->clearSubsystem();
        }

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
    
    void Engine::passInputToGameInstance(const JumaRE::InputActionData& input)
    {
        if (m_GameInstance == nullptr)
        {
            return;
        }
        if (input.device != JumaRE::InputDevice::NONE)
        {
            if (input.button != JumaRE::InputButton::NONE)
            {
                m_GameInstance->onInputButton(input.device, input.button, input.buttonAction);
            }
            else if (input.axis != JumaRE::InputAxis::NONE)
            {
                if (JumaRE::IsInputAxis2D(input.axis))
                {
                    m_GameInstance->onInputAxis2D(input.device, input.axis, input.axisValue);
                }
                else
                {
                    m_GameInstance->onInputAxis(input.device, input.axis, input.axisValue[0]);
                }
            }
        }
    }
}
