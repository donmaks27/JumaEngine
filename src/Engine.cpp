// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/Engine.h"

#include <chrono>
#include <JumaRE/RenderEngineImpl.h>
#include <JumaRE/RenderPipeline.h>

#include "JumaEngine/subsystems/meshes/MeshesSubsystem.h"
#include "JumaEngine/subsystems/shaders/ShadersSubsystem.h"
#include "JumaEngine/subsystems/textures/TexturesSubsystem.h"
#include "JumaEngine/subsystems/ui/ImageWidget.h"
#include "JumaEngine/subsystems/ui/OverlayWidget.h"
#include "JumaEngine/subsystems/ui/CursorWidget.h"
#include "JumaEngine/subsystems/ui/UISubsystem.h"
#include "JumaEngine/subsystems/ui/WidgetsCreator.h"

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
        if (createSubsystem<MeshesSubsystem>() == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to init meshes subsystem"));
            return false;
        }
        if (createSubsystem<UISubsystem>() == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to init UI subsystem"));
            return false;
        }

        m_EngineWidgetCreator = createObject<WidgetsCreator>();
        InitializeLogicObject(m_EngineWidgetCreator);

        JumaRE::WindowController* windowController = m_RenderEngine->getWindowController();
        windowController->onWindowCreated.bind(this, &Engine::onWindowCreated);
        windowController->onWindowDestroying.bind(this, &Engine::onWindowDestroying);
        for (const auto& windowID : windowController->getWindowIDs())
        {
	        onWindowCreated(windowController, windowController->findWindowData(windowID));
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
        onEngineLoopStarted();
        JUTILS_LOG(info, JSTR("Engine loop started"));

        while (!shouldExit())
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
        onEngineLoopStopped();
        JUTILS_LOG(info, JSTR("Engine loop stopped"));

        clear();
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
    void Engine::onEngineLoopStarted()
    {
        StartLogicObject(m_EngineWidgetCreator);
    }
    bool Engine::shouldExit()
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
    void Engine::onEngineLoopStopped()
    {
        JumaRE::WindowController* windowController = m_RenderEngine->getWindowController();
        windowController->onWindowCreated.unbind(this, &Engine::onWindowCreated);
        windowController->onWindowDestroying.unbind(this, &Engine::onWindowDestroying);
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

        for (const auto& subsystem : m_EngineSubsystems)
        {
            subsystem.value->clearSubsystem();
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

    void Engine::onWindowCreated(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData)
    {
		JumaRE::RenderTarget* windowRenderTarget = m_RenderEngine->getRenderTarget(windowData->windowRenderTargetID);
        JumaRE::RenderTarget* renderTarget = m_RenderEngine->createRenderTarget(windowRenderTarget->getColorFormat(), windowRenderTarget->getSize(), windowRenderTarget->getSampleCount());
        windowRenderTarget->setSampleCount(JumaRE::TextureSamples::X1);
        m_RenderEngine->getRenderPipeline()->addRenderTargetDependecy(windowData->windowRenderTargetID, renderTarget->getID());

        ImageWidget* imageWidget = m_EngineWidgetCreator->createWidget<ImageWidget>();
        imageWidget->setUsingSolidColor(false);
        imageWidget->setTexture(renderTarget);
        if (m_RenderEngine->getRenderAPI() == JumaRE::RenderAPI::OpenGL)
        {
            imageWidget->setTextureScale({ 1.0f, -1.0f });
        }

        CursorWidget* cursorWidget = m_EngineWidgetCreator->createWidget<CursorWidget>();

        OverlayWidget* overlayWidget = m_EngineWidgetCreator->createWidget<OverlayWidget>();
        overlayWidget->addWidget(imageWidget);
        overlayWidget->addWidget(cursorWidget);

        WidgetContext* widgetContext = m_EngineWidgetCreator->createWidgetContext(windowRenderTarget);
        m_EngineWidgetCreator->setRootWidget(widgetContext, overlayWidget);

        m_WindowProxyRenderTargets.add(windowData->windowRenderTargetID, { renderTarget, widgetContext });
    }
    void Engine::onWindowDestroying(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData)
    {
        const WindowProxyRenderTarget* renderTarget = m_WindowProxyRenderTargets.find(windowData->windowID);
        if (renderTarget != nullptr)
        {
            Widget* widget = renderTarget->widgetContext->getRootWidget();
            m_EngineWidgetCreator->destroyWidgetContext(renderTarget->widgetContext);
            m_EngineWidgetCreator->destroyWidget(widget, true);

            m_RenderEngine->destroyRenderTarget(renderTarget->proxyRenderTarget);

            m_WindowProxyRenderTargets.remove(windowData->windowID);
        }
    }
    JumaRE::RenderTarget* Engine::getWindowRenderTarget(const JumaRE::window_id windowID) const
    {
        const WindowProxyRenderTarget* renderTarget = m_WindowProxyRenderTargets.find(windowID);
        if (renderTarget != nullptr)
        {
	        return renderTarget->proxyRenderTarget;
        }

        const JumaRE::WindowData* windowData = m_RenderEngine != nullptr ? m_RenderEngine->getWindowController()->findWindowData(windowID) : nullptr;
        return windowData != nullptr ? m_RenderEngine->getRenderTarget(windowData->windowRenderTargetID) : nullptr;
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
