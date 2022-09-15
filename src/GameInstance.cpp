// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../include/JumaEngine/GameInstance.h"

namespace JumaEngine
{
    GameInstance::~GameInstance()
    {
        clearData_GameInstance();
    }

    bool GameInstance::init(JumaRE::RenderTarget* renderTarget)
    {
        if (renderTarget == nullptr)
        {
            JUTILS_LOG(error, JSTR("Invalid render target for game instance"));
            return false;
        }

        m_GameRenderTarget = renderTarget;
        return initInternal();
    }
    bool GameInstance::initInternal()
    {
        return true;
    }

    void GameInstance::clearInternal()
    {
        clearData_GameInstance();
    }
    void GameInstance::clearData_GameInstance()
    {
        m_GameRenderTarget = nullptr;
    }

    void GameInstance::update()
    {
    }
}
