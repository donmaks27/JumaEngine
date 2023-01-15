// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"

namespace JumaEngine
{
    class LogicObject;

    class ILogicObjectOwner
    {
    protected:
        ILogicObjectOwner() = default;
    public:
        virtual ~ILogicObjectOwner() = default;

    protected:

        static void InitializeLogicObject(LogicObject* object);
        static void StartLogicObject(LogicObject* object);
        static void UpdateLogicObject(LogicObject* object, float deltaTime);
        static void PreRenderLogicObject(LogicObject* object);
        static void StopLogicObject(LogicObject* object);
        static void DestroyLogicObject(LogicObject* object);
    };
}
