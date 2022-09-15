// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "Engine.h"

namespace JumaEngine
{
    class GameInstance;

    class GameEngineBase : public Engine
    {
        using Super = Engine;

    public:
        GameEngineBase() = default;
        virtual ~GameEngineBase() override;

        GameInstance* getGameInstance() const { return m_GameInstance; }
        template<typename T, TEMPLATE_ENABLE(is_base<GameInstance, T>)>
        T* getGameInstance() const { return dynamic_cast<T*>(getGameInstance()); }

    protected:

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<GameInstance, T>)>
        bool createGameInstance()
        {
            m_GameInstance = createObject<T>();
            return m_GameInstance != nullptr;
        }
        bool initGameInstance(JumaRE::RenderTarget* renderTarget);
        void destroyGameInstance();

        virtual void clearInternal() override;

    private:

        GameInstance* m_GameInstance = nullptr;


        void clearData_GameEngineBase();
    };
}
