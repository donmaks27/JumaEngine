// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"

namespace JumaEngine
{
    class EngineObject;

    class GameComponent : public EngineContextObject
    {
    public:
        GameComponent() = default;
        virtual ~GameComponent() override = default;

        EngineObject* getOwnerObject() const { return m_OwnerObject; }

    private:

        EngineObject* m_OwnerObject = nullptr;
    };
}
