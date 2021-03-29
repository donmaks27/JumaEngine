// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "framework/gameObject/WorldContextObject.h"

namespace JumaEngine
{
    class EngineWorld;
    class GameObject;

    class GameComponent : public WorldContextObject
    {
        friend EngineWorld;

    public:
        GameComponent() = default;
        virtual ~GameComponent() override = default;

        GameObject* getOwnerGameObject() const { return m_OwnerObject; }

    protected:

        virtual void onOwnerGameObjectChange() {}

		virtual void init() {}
    
    private:

        GameObject* m_OwnerObject = nullptr;
    };
}
