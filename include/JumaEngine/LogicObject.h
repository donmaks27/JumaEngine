// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "EngineContextObject.h"

namespace JumaEngine
{
    class LogicObject : public EngineContextObject
    {
        JUMAENGINE_CLASS_ABSTRACT(LogicObject, EngineContextObject)

        friend class Engine;

    public:
        LogicObject() = default;
        virtual ~LogicObject() override = default;

    protected:

        virtual bool initLogic() { return true; }
        virtual void startLogic() {}
        virtual void clearLogic() {}

        virtual void update(float deltaTime) {}
        virtual void postUpdate() {}
    };
}
