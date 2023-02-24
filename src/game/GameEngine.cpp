// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/game/GameEngine.h"

#ifdef JUMAENGINE_MODE_GAME

#include "JumaEngine/engine/ConfigEngineSubsystem.h"
#include "JumaEngine/render/RenderEngineSubsystem.h"

namespace JumaEngine
{
    JumaRE::RenderAPI GameEngine::getDesiredRenderAPI() const
    {
        const ConfigEngineSubsystem* configSubsytem = getSubsystem<ConfigEngineSubsystem>();
        if (configSubsytem == nullptr)
        {
            return Super::getDesiredRenderAPI();
        }

        jstring renderAPIString;
        if (!configSubsytem->getValue(JSTR("game"), JSTR("General"), JSTR("renderAPI"), renderAPIString))
        {
            return Super::getDesiredRenderAPI();
        }

        if (renderAPIString == JSTR("Vulkan"))
        {
            return JumaRE::RenderAPI::Vulkan;
        }
        if (renderAPIString == JSTR("OpenGL"))
        {
            return JumaRE::RenderAPI::OpenGL;
        }
        if (renderAPIString == JSTR("DirectX11"))
        {
            return JumaRE::RenderAPI::DirectX11;
        }
        if (renderAPIString == JSTR("DirectX12"))
        {
            return JumaRE::RenderAPI::DirectX12;
        }
        return Super::getDesiredRenderAPI();
    }
    jstring GameEngine::getWindowsTitle() const
    {
        const ConfigEngineSubsystem* configSubsytem = getSubsystem<ConfigEngineSubsystem>();
        if (configSubsytem == nullptr)
        {
            return Super::getWindowsTitle();
        }
        jstring title;
        if (!configSubsytem->getValue(JSTR("game"), JSTR("General"), JSTR("title"), title))
        {
            return Super::getWindowsTitle();
        }
        return title;
    }

    JumaRE::RenderTarget* GameEngine::getGameInstanceRenderTarget() const
    {
        const JumaRE::RenderEngine* renderEngine = getRenderEngine();
        const JumaRE::WindowController* windowController = renderEngine != nullptr ? renderEngine->getWindowController() : nullptr;
        const JumaRE::window_id windowID = windowController != nullptr ? windowController->getMainWindowID() : JumaRE::window_id_INVALID;
        if (windowID == JumaRE::window_id_INVALID)
        {
	        return nullptr;
        }

        const RenderEngineSubsystem* renderSubsystem = getSubsystem<RenderEngineSubsystem>();
        return renderSubsystem != nullptr ? renderSubsystem->getWindowRenderTarget(windowID) : nullptr;
    }

    bool GameEngine::onEngineLoopStarting()
    {
        if (!Super::onEngineLoopStarting())
        {
            return false;
        }

        JumaRE::WindowController* windowController = getRenderEngine()->getWindowController();
        windowController->onWindowInput.bind(this, &GameEngine::onWindowInput);
        InitializeEngineObject(getGameInstance());
        return true;
    }
    void GameEngine::onEngineLoopStarted()
    {
        Super::onEngineLoopStarted();

        ActivateEngineObject(getGameInstance());
    }
    void GameEngine::update(const float deltaTime)
    {
        Super::update(deltaTime);
        
        UpdateEngineObject(getGameInstance(), deltaTime);
    }
    void GameEngine::preRender()
    {
        Super::preRender();

        PreRenderEngineObject(getGameInstance());
    }
    void GameEngine::onEngineLoopStopped()
    {
        getRenderEngine()->getWindowController()->onWindowInput.unbind(this, &GameEngine::onWindowInput);

        ClearEngineObject(getGameInstance());

        Super::onEngineLoopStopped();
    }

    void GameEngine::onWindowInput(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
        const JumaRE::InputActionData& input)
    {
        passInputToGameInstance(input);
    }
}

#endif
