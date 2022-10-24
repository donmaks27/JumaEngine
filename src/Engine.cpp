// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../include/JumaEngine/Engine.h"

#include <JumaRE/RenderEngineImpl.h>

#include "../include/JumaEngine/EngineContextObject.h"
#include "../include/JumaEngine/GameInstance.h"
#include "../include/JumaEngine/subsystems/shaders/ShadersSubsystem.h"

namespace JumaEngine
{
    Engine::~Engine()
    {
        clearData_Engine();
    }

    bool Engine::init()
    {
        return initInternal();
    }
    bool Engine::initInternal()
    {
        return (getSubsystem<ShadersSubsystem>() != nullptr);
    }

    void Engine::clear()
    {
        clearInternal();
    }
    void Engine::clearInternal()
    {
        clearData_Engine();
    }
    void Engine::clearData_Engine()
    {
        destroyGameInstance();
        destroyEngineSubsystems();
        destroyRenderEngine();
    }

    bool Engine::createRenderEngine(const JumaRE::RenderAPI api, const JumaRE::WindowCreateInfo& mainWindowInfo)
    {
        JumaRE::RenderEngine* renderEngine = JumaRE::CreateRenderEngine(api);
        if (renderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create render engine ({})"), api);
            return false;
        }
        if (!renderEngine->init(mainWindowInfo))
        {
            JUTILS_LOG(error, JSTR("Failed to initialize render engine ({})"), api);
            delete renderEngine;
            return false;
        }

        JUTILS_LOG(correct, JSTR("Render engine ({}) initialized"), api);
        m_RenderEngine = renderEngine;

        JumaRE::WindowController* windowController = m_RenderEngine->getWindowController();
        windowController->OnInputButton.bind(this, &Engine::onInputButton);
        windowController->OnInputAxis.bind(this, &Engine::onInputAxis);
        windowController->OnInputAxis2D.bind(this, &Engine::onInputAxis2D);
        return true;
    }
    void Engine::destroyRenderEngine()
    {
        if (m_RenderEngine != nullptr)
        {
            JumaRE::WindowController* windowController = m_RenderEngine->getWindowController();
            windowController->OnInputButton.unbind(this, &Engine::onInputButton);
            windowController->OnInputAxis.unbind(this, &Engine::onInputAxis);
            windowController->OnInputAxis2D.unbind(this, &Engine::onInputAxis2D);

            delete m_RenderEngine;
            m_RenderEngine = nullptr;
        }
    }

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

    GameInstance* Engine::createGameInstance(const EngineSubclass<GameInstance>& subclass)
    {
        if (m_GameInstance != nullptr)
        {
            return m_GameInstance;
        }

        m_GameInstance = createObject(subclass);
        if (!initGameInstance())
        {
            delete m_GameInstance;
            m_GameInstance = nullptr;
        }
        return m_GameInstance;
    }
    void Engine::destroyGameInstance()
    {
        if (m_GameInstance != nullptr)
        {
            m_GameInstance->clear();
            delete m_GameInstance;
            m_GameInstance = nullptr;
        }
    }

    EngineSubsystem* Engine::getSubsystem(const EngineSubclass<EngineSubsystem>& subclass)
    {
        if (subclass == nullptr)
        {
            return nullptr;
        }
        EngineSubsystem* const* subsystemPtr = m_EngineSubsystems.find(subclass);
        if (subsystemPtr != nullptr)
        {
            return *subsystemPtr;
        }
        EngineSubsystem* subsystem = createObject(subclass);
        if (subsystem == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create subsystem {}"), subclass.get()->getClassName());
            return nullptr;
        }

        m_EngineSubsystems.add(subclass, subsystem);
        if (!subsystem->initSubsystem())
        {
            JUTILS_LOG(error, JSTR("Failed to init subsystem {}"), subclass.get()->getClassName());
            m_EngineSubsystems.remove(subclass);
            delete subsystem;
            return nullptr;
        }
        return subsystem;
    }
    void Engine::destroyEngineSubsystems()
    {
        for (const auto& subsystem : m_EngineSubsystems)
        {
            delete subsystem.value;
        }
        m_EngineSubsystems.clear();
    }

    void Engine::onInputButton(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData,
        const JumaRE::InputDevice device, const JumaRE::InputButton button, const JumaRE::InputButtonAction action)
    {
        GameInstance* gameInstance = getGameInstance();
        if (gameInstance != nullptr)
        {
            gameInstance->onInputButton(device, button, action);
        }
    }
    void Engine::onInputAxis(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData,
        const JumaRE::InputDevice device, const JumaRE::InputAxis axis, const float value)
    {
        GameInstance* gameInstance = getGameInstance();
        if (gameInstance != nullptr)
        {
            gameInstance->onInputAxis(device, axis, value);
        }
    }
    void Engine::onInputAxis2D(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData,
        const JumaRE::InputDevice device, const JumaRE::InputAxis axis, const math::vector2& value)
    {
        GameInstance* gameInstance = getGameInstance();
        if (gameInstance != nullptr)
        {
            gameInstance->onInputAxis2D(device, axis, value);
        }
    }
}
