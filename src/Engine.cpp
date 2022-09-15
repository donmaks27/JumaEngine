// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../include/JumaEngine/Engine.h"

#include <JumaRE/RenderEngineImpl.h>
#include <JumaRE/RenderPipeline.h>

#include "../include/JumaEngine/EngineContextObject.h"

namespace JumaEngine
{
    Engine::~Engine()
    {
        clearData_Engine();
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
            return false;
        }

        JUTILS_LOG(correct, JSTR("Render engine ({}) initialized"), api);
        m_RenderEngine = renderEngine;
        return true;
    }
    void Engine::destroyRenderEngine()
    {
        if (m_RenderEngine != nullptr)
        {
            delete m_RenderEngine;
            m_RenderEngine = nullptr;
        }
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
        m_RenderEngine = nullptr;
    }

    void Engine::registerObjectInternal(EngineContextObject* object)
    {
        if (object != nullptr)
        {
            object->m_Engine = this;
        }
    }
}
