// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

namespace JumaEngine
{
    class EngineObject;

    class IEngineObjectOwner
    {
    protected:
        IEngineObjectOwner() = default;
    public:
        virtual ~IEngineObjectOwner() = default;

    protected:

        static void InitializeLogicObject(EngineObject* object);
        static void StartLogicObject(EngineObject* object);
        static void UpdateLogicObject(EngineObject* object, float deltaTime);
        static void PreRenderLogicObject(EngineObject* object);
        static void StopLogicObject(EngineObject* object);
        static void DestroyLogicObject(EngineObject* object);
    };
}
