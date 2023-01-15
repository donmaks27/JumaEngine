// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/LogicObject.h"

namespace JumaEngine
{
    void LogicObject::initializeLogicObject()
    {
        if (!m_ObjectInitialized)
        {
            m_ObjectInitialized = true;
            onInitialized();
        }
    }

    void LogicObject::startLogic()
    {
        if (isInitialized() && !isLogicActive())
        {
            m_LogicStarted = true;
            onLogicStarted();
        }
    }

    void LogicObject::update(const float deltaTime)
    {
        if (isLogicActive())
        {
            onUpdate(deltaTime);
        }
    }
    void LogicObject::preRender()
    {
        if (isLogicActive())
        {
            onPreRender();
        }
    }

    void LogicObject::stopLogic()
    {
        if (isLogicActive())
        {
            onLogicStopping();
            m_LogicStarted = false;
        }
    }

    void LogicObject::destroyLogicObject()
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
