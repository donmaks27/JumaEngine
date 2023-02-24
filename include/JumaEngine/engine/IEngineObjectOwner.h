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

        static void InitializeEngineObject(EngineObject* object);
        static void ActivateEngineObject(EngineObject* object);
        static void UpdateEngineObject(EngineObject* object, float deltaTime);
        static void PreRenderEngineObject(EngineObject* object);
        static void DeactivateEngineObject(EngineObject* object);
        static void ClearEngineObject(EngineObject* object);
    };
}
