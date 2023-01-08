// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"
#include "EngineContextObject.h"

namespace JumaEngine
{
    class ILogicObjectOwner;

    class LogicObject : public EngineContextObject
    {
        JUMAENGINE_CLASS(LogicObject, EngineContextObject)

        friend ILogicObjectOwner;

    public:
        LogicObject() = default;
        virtual ~LogicObject() override = default;

        bool isInitialized() const { return m_ObjectInitialized && !isDestroyed(); }
        bool isLogicActive() const { return m_LogicStarted; }
        bool isDestroyed() const { return m_ObjectDestroyed; }

    protected:

        virtual void onInitialized() {}
        virtual void onLogicStarted() {}
        virtual void onUpdate(float deltaTime) {}
        virtual void onPostUpdate() {}
        virtual void onLogicStopping() {}
        virtual void onDestroying() {}

    private:

        bool m_ObjectInitialized = false;
        bool m_LogicStarted = false;
        bool m_ObjectDestroyed = false;


        void initializeLogicObject();
        void startLogic();
        void update(float deltaTime);
        void postUpdate();
        void stopLogic();
        void destroyLogicObject();
    };
}
