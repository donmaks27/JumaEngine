// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include "utils/jarray.h"

namespace JumaEngine
{
    class GameComponent;
    class SceneComponent;

    class EngineObject : public EngineContextObject
    {
    public:
        EngineObject() = default;
        virtual ~EngineObject() override = default;

    private:

        jarray<GameComponent*> m_Components;
    };
}
