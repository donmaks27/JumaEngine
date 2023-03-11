// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/engine/Engine.h"

#include <chrono>
#include <JumaRE/RenderEngineImpl.h>
#include <JumaRE/RenderPipeline.h>

#include "JumaEngine/assets/AssetsEngineSubsystem.h"
#include "JumaEngine/engine/ConfigEngineSubsystem.h"
#include "JumaEngine/game/GameInstance.h"
#include "JumaEngine/render/RenderEngineSubsystem.h"
#include "JumaEngine/widgets/WidgetsCreator.h"

namespace JumaEngine
{
	jdescriptor_table_pointer<> Engine::createObjectDescriptor(const EngineClass* objectClass)
	{
        if (objectClass == nullptr)
        {
	        return nullptr;
        }
        EngineContextObject* object = objectClass->createBaseObject();
        if (object == nullptr)
        {
	        return nullptr;
        }
        const jdescriptor_table_pointer<> pointer = m_EngineObjectDescriptors.createDescriptor(object);
        object->m_ObjectDescriptor = pointer;
		return pointer;
	}
    void Engine::onEngineObjectDescriptorDestroying(EngineContextObject* object)
    {
        object->onObjectDescriptorDestroying();
    }
    void Engine::onEngineObjectDestroying(EngineObject* object)
    {
        m_DestroyingEngineObjects.add(object->m_ObjectDescriptor);
    }

    bool Engine::init(const EngineSubclass<GameInstance>& gameInstanceClass)
    {
        if (m_GameInstance != nullptr)
        {
            JUTILS_LOG(warning, JSTR("Engine already initialized!"));
            return false;
        }

        EngineObjectPtrBase::s_Engine = this;
        m_EngineObjectDescriptors.onObjectDestroying.bind(this, &Engine::onEngineObjectDescriptorDestroying);

        if (!initEngine())
        {
            JUTILS_LOG(error, JSTR("Failed to init engine"));
            clear();
            return false;
        }

        m_GameInstance = createObject(gameInstanceClass);
        if (m_GameInstance == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to init game instance"));
            clear();
            return false;
        }

        if (!initRenderEngine())
        {
            JUTILS_LOG(error, JSTR("Failed to init render engine"));
            clear();
            return false;
        }
        return true;
    }
    bool Engine::initEngine()
    {
        const ConfigEngineSubsystem* configSubsystem = createSubsystem<ConfigEngineSubsystem>();
        if (configSubsystem == nullptr)
        {
	        JUTILS_LOG(error, JSTR("Failed to init ConfigEngineSubsystem"));
            return false;
        }
        return true;
    }
    bool Engine::initRenderEngine()
    {
        JumaRE::RenderAPI renderAPI = getDesiredRenderAPI();
        if (!JumaRE::IsSupportRenderAPI(renderAPI))
        {
	        if (JumaRE::IsSupportRenderAPI(JumaRE::RenderAPI::Vulkan))
	        {
		        renderAPI = JumaRE::RenderAPI::Vulkan;
	        }
            else if (JumaRE::IsSupportRenderAPI(JumaRE::RenderAPI::OpenGL))
	        {
		        renderAPI = JumaRE::RenderAPI::OpenGL;
	        }
            else if (JumaRE::IsSupportRenderAPI(JumaRE::RenderAPI::DirectX11))
	        {
		        renderAPI = JumaRE::RenderAPI::DirectX11;
	        }
            else if (JumaRE::IsSupportRenderAPI(JumaRE::RenderAPI::DirectX12))
	        {
		        renderAPI = JumaRE::RenderAPI::DirectX12;
	        }
        }
        
        m_RenderEngine = JumaRE::CreateRenderEngine(renderAPI);
        if (m_RenderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create render engine ({})"), renderAPI);
            return false;
        }
        if (!m_RenderEngine->init({ getWindowsTitle(), { 800, 600 } }))
        {
            JUTILS_LOG(error, JSTR("Failed to init render engine ({})"), renderAPI);
            delete m_RenderEngine;
            m_RenderEngine = nullptr;
            return false;
        }
        JUTILS_LOG(info, JSTR("Render engine initialized ({})"), renderAPI);
        
        RenderEngineSubsystem* renderSubsystem = createSubsystem<RenderEngineSubsystem>();
        if (renderSubsystem == nullptr)
        {
	        JUTILS_LOG(error, JSTR("Failed to init RenderEngineSubsystem"));
            return false;
        }
        if (createSubsystem<AssetsEngineSubsystem>() == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to init AssetsEngineSubsystem"));
            return false;
        }

        m_EngineWidgetCreator = createObject<WidgetsCreator>();
        InitializeEngineObject(m_EngineWidgetCreator.get());

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

            for (const auto& pointer : m_DestroyingEngineObjects)
            {
                m_EngineObjectDescriptors.destroy(pointer);
            }
            m_DestroyingEngineObjects.clear();
            m_EngineObjectDescriptors.cleanup();

            preRender();
            if (!m_RenderEngine->render())
            {
                JUTILS_LOG(error, JSTR("Render failed"));
                break;
            }
        }
        getRenderEngine()->getRenderPipeline()->waitForRenderFinished();
        
        JUTILS_LOG(info, JSTR("Engine loop stopped"));
        onEngineLoopStopped();

        clear();
    }
    bool Engine::onEngineLoopStarting()
    {
        EngineObjectPtr<RenderTarget> gameInstanceRT = getGameInstanceRenderTarget();
        if (gameInstanceRT == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invalid game instance render target"));
            return false;
        }
        
        m_GameInstance->setupRenderTarget(std::move(gameInstanceRT));
        return true;
    }
    void Engine::onEngineLoopStarted()
    {
        ActivateEngineObject(m_EngineWidgetCreator.get());
    }
    bool Engine::shouldStopEngineLoop()
    {
        return (m_RenderEngine != nullptr) && m_RenderEngine->getWindowController()->isMainWindowClosed();
    }
    void Engine::update(const float deltaTime)
    {
        UpdateEngineObject(m_EngineWidgetCreator.get(), deltaTime);
    }
    void Engine::preRender()
    {
        PreRenderEngineObject(m_EngineWidgetCreator.get());
    }
    void Engine::onEngineLoopStopped()
    {
        getSubsystem<RenderEngineSubsystem>()->destroyProxyWindowRenderTargets();
        ClearEngineObject(m_EngineWidgetCreator.get());
    }

    void Engine::clear()
    {
        clearRenderEngine();
        if (m_GameInstance != nullptr)
        {
            m_EngineObjectDescriptors.destroy(m_GameInstance.m_ObjectPointer);
            m_GameInstance = nullptr;
        }
        clearEngine();

        EngineObjectPtrBase::s_Engine = nullptr;
    }
    void Engine::clearRenderEngine()
    {
        if (m_EngineWidgetCreator != nullptr)
        {
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
	            renderSubsystem = subsystem.value.get();
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
        m_EngineSubsystems.clear();
        
        m_EngineObjectDescriptors.clear();
        m_EngineObjectDescriptors.onObjectDestroying.unbind(this, &Engine::onEngineObjectDescriptorDestroying);
    }

    EngineSubsystem* Engine::createSubsystem(const EngineSubclass<EngineSubsystem>& subsystemClass)
    {
        if (subsystemClass == nullptr)
        {
            return nullptr;
        }
        const EngineObjectPtr<EngineSubsystem>* subsystemPtr = m_EngineSubsystems.find(subsystemClass);
        if (subsystemPtr != nullptr)
        {
            return subsystemPtr->get();
        }
        const EngineObjectPtr<EngineSubsystem> subsystem = createObject(subsystemClass);
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
            return nullptr;
        }
        return subsystem.get();
    }
    EngineSubsystem* Engine::getSubsystem(const EngineSubclass<EngineSubsystem>& subsystemClass) const
    {
        if (subsystemClass == nullptr)
        {
            return nullptr;
        }
        const EngineObjectPtr<EngineSubsystem>* subsystemPtr = m_EngineSubsystems.find(subsystemClass);
        return subsystemPtr != nullptr ? subsystemPtr->get() : nullptr;
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
