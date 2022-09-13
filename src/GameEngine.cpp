// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../include/JumaEngine/GameEngine.h"

namespace JumaEngine
{
    bool GameEngine::createRenderEngine(const JumaRE::RenderAPI api)
    {
        JumaRE::RenderEngine* renderEngine = JumaRE::CreateRenderEngine(api);
        if (renderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Failed to create render engine ({})"), api);
            return false;
        }
        if (!renderEngine->init({ JSTR("JumaEngine"), { 800, 600 }, JumaRE::TextureSamples::X1 }))
        {
            JUTILS_LOG(error, JSTR("Failed to initialize render engine ({})"), api);
            return false;
        }

        JUTILS_LOG(correct, JSTR("Render engine ({}) initialized"), api);
        m_RenderEngine = renderEngine;
        return true;
    }
}
