// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/engine/EngineObject.h"

namespace JumaEngine
{
    void EngineObject::initializeLogicObject()
    {
        if (!m_ObjectInitialized)
        {
            m_ObjectInitialized = true;
            onInitialized();
        }
    }

    void EngineObject::startLogic()
    {
        if (isInitialized() && !isLogicActive())
        {
            m_LogicStarted = true;
            onLogicStarted();
        }
    }

    void EngineObject::update(const float deltaTime)
    {
        if (isLogicActive())
        {
            onUpdate(deltaTime);
        }
    }
    void EngineObject::preRender()
    {
        if (isLogicActive())
        {
            onPreRender();
        }
    }

    void EngineObject::stopLogic()
    {
        if (isLogicActive())
        {
            onLogicStopping();
            m_LogicStarted = false;
        }
    }

    void EngineObject::destroyLogicObject()
    {
        if (!isDestroyed())
        {
            stopLogic();
            if (isInitialized())
            {
                onDestroying();
                m_ObjectDestroyed = true;
            }
        }
    }
}
