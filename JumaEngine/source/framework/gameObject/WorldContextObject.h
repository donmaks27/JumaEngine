// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"

namespace JumaEngine
{
    class EngineWorld;

    class WorldContextObject : public EngineContextObject
    {
        JUMAENGINE_CLASS(WorldContextObject, EngineContextObject)

        friend EngineWorld;

    public:
        WorldContextObject() = default;
        virtual ~WorldContextObject() override = default;

        EngineWorld* getOwnerWorld() const { return m_World; }
    
    protected:

        virtual void onRegisterInWorld() {}

		virtual void onGameStarted() {}

    	virtual void tick(double deltaTime) {}

    private:

        EngineWorld* m_World = nullptr;
    };
}
