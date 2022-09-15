// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "GameEngineBase.h"

namespace JumaEngine
{
    class GameEngine final : public GameEngineBase
    {
        using Super = GameEngineBase;

    public:
        GameEngine() = default;
        virtual ~GameEngine() override;

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<GameInstance, T>)>
        bool init();

        virtual bool update() override;

    protected:

        virtual void clearInternal() override;

    private:

        bool initInternal();

        void clearData_GameEngine();
    };

    template <typename T, TEMPLATE_ENABLE_IMPL(is_base_and_not_abstract<GameInstance, T>)>
    bool GameEngine::init()
    {
        if (!createRenderEngine(JumaRE::RenderAPI::Vulkan, { 
                JSTR("JumaEngine"), { 800, 600 }
            }) || !createGameInstance<T>())
        {
            return false;
        }
        return initInternal();
    }
}
