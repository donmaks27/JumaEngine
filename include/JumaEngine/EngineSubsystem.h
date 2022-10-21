// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "EngineContextObject.h"

namespace JumaEngine
{
    class EngineSubsystem : public EngineContextObject
    {
        JUMAENGINE_CLASS_ABSTRACT(EngineSubsystem, EngineContextObject)

    public:
        EngineSubsystem() = default;
        virtual ~EngineSubsystem() override = default;
    };
}
