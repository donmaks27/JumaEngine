// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/ILogicObjectOwner.h"

#include "JumaEngine/LogicObject.h"

namespace JumaEngine
{
    bool ILogicObjectOwner::CallInitLogicObject(LogicObject* object)
    {
        if (object->isLogicInitialized())
        {
            return true;
        }
        if (!object->initLogicObject())
        {
            return false;
        }
        object->m_LogicObjectInitialized = true;
        return true;
    }
    bool ILogicObjectOwner::CallOnStartLogic(LogicObject* object)
    {
        if (!CallInitLogicObject(object))
        {
            return false;
        }
        if (!object->isLogicStarted())
        {
            object->onStartLogic();
            object->m_LogicStarted = true;
        }
        return true;
    }

    void ILogicObjectOwner::CallUpdateLogicObject(LogicObject* object, const float deltaTime)
    {
        object->update(deltaTime);
    }
    void ILogicObjectOwner::CallPostUpdateLogicObject(LogicObject* object)
    {
        object->postUpdate();
    }

    void ILogicObjectOwner::CallOnStopLogic(LogicObject* object)
    {
        if (object->isLogicStarted())
        {
            object->onStopLogic();
            object->m_LogicStarted = false;
        }
    }
    void ILogicObjectOwner::CallClearLogicObject(LogicObject* object)
    {
        CallOnStopLogic(object);
        if (object->isLogicInitialized())
        {
            object->clearLogicObject();
            object->m_LogicObjectInitialized = false;
        }
    }
}
