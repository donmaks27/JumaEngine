// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/engine/EngineObject.h"

namespace JumaEngine
{
    void EngineObject::initializeEngineObject()
    {
        if (!m_ObjectInitialized)
        {
            m_ObjectInitialized = true;
            onInitialized();
        }
    }

    void EngineObject::activateEngineObject()
    {
        if (isInitialized() && !isActive())
        {
            m_ObjectActivated = true;
            onActivated();
        }
    }

    void EngineObject::update(const float deltaTime)
    {
        if (isActive())
        {
            onUpdate(deltaTime);
        }
    }
    void EngineObject::preRender()
    {
        if (isActive())
        {
            onPreRender();
        }
    }

    void EngineObject::deactivateEngineObject()
    {
        if (isActive())
        {
            onDeactivate();
            m_ObjectActivated = false;
        }
    }

    void EngineObject::clearEngineObject()
    {
        if (!isDestroyed())
        {
            deactivateEngineObject();
            if (isInitialized())
            {
                onClear();
                m_ObjectDestroyed = true;
            }
        }
    }
}
