// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../include/JumaEngine/GameEngineBase.h"

#include "../include/JumaEngine/GameInstance.h"

namespace JumaEngine
{
    GameEngineBase::~GameEngineBase()
    {
        clearData_GameEngineBase();
    }

    bool GameEngineBase::initGameInstance(JumaRE::RenderTarget* renderTarget)
    {
        return (m_GameInstance != nullptr) && m_GameInstance->init(renderTarget);
    }

    void GameEngineBase::destroyGameInstance()
    {
        if (m_GameInstance != nullptr)
        {
            m_GameInstance->clear();
            delete m_GameInstance;
            m_GameInstance = nullptr;
        }
    }

    void GameEngineBase::clearInternal()
    {
        clearData_GameEngineBase();
        Super::clearInternal();
    }
    void GameEngineBase::clearData_GameEngineBase()
    {
    }
}
