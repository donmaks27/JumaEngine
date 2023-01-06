// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "EngineContextObject.h"

namespace JumaEngine
{
    class ILogicObjectOwner;

    class LogicObject : public EngineContextObject
    {
        JUMAENGINE_CLASS_ABSTRACT(LogicObject, EngineContextObject)

        friend ILogicObjectOwner;

    public:
        LogicObject() = default;
        virtual ~LogicObject() override = default;

        bool isLogicInitialized() const { return m_LogicObjectInitialized; }
        bool isLogicStarted() const { return m_LogicStarted; }

    protected:

        virtual bool initLogicObject() { return true; }
        virtual void onStartLogic() {}
        virtual void update(float deltaTime) {}
        virtual void postUpdate() {}
        virtual void onStopLogic() {}
        virtual void clearLogicObject() {}

    private:

        bool m_LogicObjectInitialized = false;
        bool m_LogicStarted = false;
    };
}
