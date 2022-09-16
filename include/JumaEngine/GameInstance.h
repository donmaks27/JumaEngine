// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "EngineContextObject.h"

#include <JumaRE/RenderTarget.h>

namespace JumaEngine
{
    class GameInstance : public EngineContextObject
    {
        using Super = EngineContextObject;

        friend Engine;

    public:
        GameInstance() = default;
        virtual ~GameInstance() override;
        
        bool init(JumaRE::RenderTarget* renderTarget);
        void clear();

        virtual void update();

    protected:

        virtual bool initInternal();
        virtual void clearInternal();

        JumaRE::RenderTarget* getGameRenderTarget() const { return m_GameRenderTarget; }

    private:

        JumaRE::RenderTarget* m_GameRenderTarget = nullptr;


        void clearData_GameInstance();
    };
}
