// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "EngineContextObject.h"

#include <JumaRE/RenderTarget.h>

namespace JumaEngine
{
    class GameEngineBase;

    class GameInstance : public EngineContextObject
    {
        using Super = EngineContextObject;

        friend GameEngineBase;

    public:
        GameInstance() = default;
        virtual ~GameInstance() override;

        virtual void update();

    protected:

        virtual bool initInternal();
        virtual void clearInternal();

        JumaRE::RenderTarget* getGameRenderTarget() const { return m_GameRenderTarget; }

    private:

        JumaRE::RenderTarget* m_GameRenderTarget = nullptr;


        bool init(JumaRE::RenderTarget* renderTarget);

        void clear() { clearInternal(); }
        void clearData_GameInstance();
    };
}
