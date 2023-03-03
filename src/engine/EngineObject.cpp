// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/engine/EngineObject.h"

#include "JumaEngine/engine/Engine.h"

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

    void EngineObject::clearEngineObject(const bool notifyEngine)
    {
        if (!isDestroyed())
        {
            if (notifyEngine)
            {
                getEngine()->onEngineObjectDestroying(this);
            }
            if (isInitialized())
            {
                deactivateEngineObject();

                onDestroying.call(this);
                onClear();
                m_ObjectDestroyed = true;
            }
        }
    }
}
