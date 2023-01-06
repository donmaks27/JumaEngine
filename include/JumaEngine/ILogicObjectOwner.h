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

        static bool CallInitLogicObject(LogicObject* object);
        static bool CallOnStartLogic(LogicObject* object);
        static void CallUpdateLogicObject(LogicObject* object, float deltaTime);
        static void CallPostUpdateLogicObject(LogicObject* object);
        static void CallOnStopLogic(LogicObject* object);
        static void CallClearLogicObject(LogicObject* object);
    };
}
